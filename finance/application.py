import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True


# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    user_id = session.get("user_id")
    # select all the rows of transactions for the user logged in and aggregate by unique symbols. Don't show stock if there are no shares.
    stocks = db.execute(
        """
        SELECT symbol, SUM(shares) AS shares
        FROM transactions
        WHERE id = :id
        GROUP BY symbol
        HAVING SUM(shares) > 0
        """,
        id=user_id)
    # create new variable to calculate stock's current value
    total_current_value = 0
    for stock in stocks:
        # obtain and store name and current_price in new keys for each stock from SQL query
        quote = lookup(stock["symbol"])
        stock["name"] = quote["name"]
        stock["current_price"] = quote["price"]
        stock["current_value"] = (stock["shares"]) * (stock["current_price"])
        total_current_value += stock["current_value"]
    # calculate cash spent
    default_cash = db.execute("SELECT * FROM users WHERE id = :id", id=user_id)
    cash_available = default_cash[0]["cash"]
    grand_total = cash_available
    transactions_done = db.execute("SELECT SUM(total_price) AS spent_cash FROM transactions WHERE id = :id", id=user_id)
    if (transactions_done[0]["spent_cash"]) is not None:
        cash_available = cash_available - (transactions_done[0]["spent_cash"])
        grand_total = cash_available + total_current_value
    return render_template("index.html", stocks=stocks, cash_available=cash_available, grand_total=grand_total)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    if request.method == "POST":
        form_symbol = request.form.get("symbol")
        form_shares = request.form.get("shares")
        # standard error handling for missing fields or negative shares
        if not form_symbol:
            return apology("must provide symbol", 400)
        elif not lookup(form_symbol):
            return apology("no symbol found", 400)
        elif not form_shares:
            return apology("must provide number of shares", 400)
        elif not form_shares.isdigit():
            return apology("integer of at least 1 is required", 400)
        elif int(form_shares) < 1:
            return apology("integer of at least 1 is required", 400)
        # set variables for easier coding
        user_id = session.get("user_id")
        quote = lookup(form_symbol)
        total_cost = float(quote["price"]) * float(form_shares)
        available = db.execute("SELECT * FROM users WHERE id = :id", id=user_id)
        # check if there's enough cash from user logged in to make requested transactions
        if float(available[0]["cash"]) < total_cost:
            return apology("not enough available cash for transactions", 400)
        else:
            # if enough money for transactions, log transactions transaction
            db.execute(
                """
                INSERT INTO transactions
                (symbol, shares, price, total_price, id)
                VALUES (:symbol, :shares, :price, :total_price, :id)
                """,
                symbol=quote["symbol"],
                shares=form_shares,
                price=quote["price"],
                total_price=total_cost, id=user_id)
        return redirect("/")
    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    user_id = session.get("user_id")
    # select all the rows of transactions for the user logged in
    stocks = db.execute("SELECT * FROM transactions WHERE id = :id", id=user_id)
    for stock in stocks:
        # create new key for each stock to store transaction type. Convert sold shares into positive value for display
        if stock["shares"] < 0:
            stock["transaction_type"] = "sell"
            stock["shares"] = -1 * stock["shares"]
        else:
            stock["transaction_type"] = "buy"

    return render_template("history.html", stocks=stocks)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)
        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)
        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))
        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)
        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]
        # Redirect user to home page
        return redirect("/")
    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "POST":
        form_symbol = request.form.get("symbol")
        if not form_symbol:
            return apology("must provide symbol", 400)
        elif not lookup(form_symbol):
            return apology("no symbol found", 400)
        quote = lookup(form_symbol)

        return render_template("quoted.html", quote=quote)

    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "POST":
        # error handling for username and password
        if not request.form.get("username"):
            return apology("must provide username", 400)
        # error handling for no password
        elif not request.form.get("password"):
            return apology("must provide password", 400)
        # error handling for no password in confirmation line
        elif not request.form.get("confirmation"):
            return apology("must provide password confirmation", 400)
        # error handling for mismatched passwords
        elif request.form.get("password") != request.form.get("confirmation"):
            return apology("passwords do not match", 400)

        # check if username already exists in users table and only insert new registrant if it's new
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))
        if len(rows) == 0:
            hashed = generate_password_hash(request.form.get("password"))
            db.execute(
                """
                INSERT INTO users (username, hash)
                VALUES (:username, :hash)
                """,
                username=request.form.get("username"), hash=hashed)
        else:
            return apology("username is already taken", 400)

        # store session
        new = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))
        session["user_id"] = new[0]["id"]
        return redirect("/")

    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    # for ease of use, flatten list of dictionaries from SQL query into dictionary with k,v being symbol:total shares owned.
    user_id = session.get("user_id")
    owned_stocks = {}
    stocks = db.execute(
        """
        SELECT symbol, SUM(shares) as total_shares
        FROM transactions
        WHERE id = :id
        GROUP BY symbol
        """,
        id=user_id)
    for stock in stocks:
        owned_stocks[stock["symbol"]] = stock["total_shares"]

    if request.method == "POST":
        form_symbol = request.form.get("symbol")
        form_shares = request.form.get("shares")
        # standard error handling for missing fields or negative shares and also if the user doesn't have enough shares to sell
        if not form_symbol:
            return apology("must provide symbol", 400)
        elif not lookup(form_symbol):
            return apology("no symbol found", 400)
        elif not form_shares:
            return apology("must provide number of shares", 400)
        elif not form_shares.isdigit():
            return apology("integer of at least 1 is required", 400)
        elif int(form_shares) < 1:
            return apology("integer of at least 1 is required", 400)
        if form_symbol not in owned_stocks.keys():
            return apology("you don't own this stock", 400)
        else:
            if int(owned_stocks[form_symbol]) < int(form_shares):
                return apology("you don't have that many to sell")

        # store values for template and since data is going into transactions table, record sell transactions in negative values
        quote = lookup(form_symbol)
        shares_delta = -1 * int(form_shares)
        cash_add = -1 * int(form_shares) * quote["price"]
        updated = db.execute(
            """
            INSERT INTO transactions
            (symbol, shares, price, total_price, id)
            VALUES (?, ?, ?, ?, ?)
            """,
            form_symbol, shares_delta, quote["price"], cash_add, user_id)

        return redirect("/")

    else:
        return render_template("sell.html", owned_stocks=owned_stocks)


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)