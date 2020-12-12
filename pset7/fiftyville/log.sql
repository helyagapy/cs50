-- Keep a log of any SQL queries you execute as you solve the mystery.
-- All I know is that the theft took place on July 28, 2020 and that it took place on Chamberlin Street. Start looking at crime scene reports for that date and location:
SELECT * FROM crime_scene_reports
WHERE year = 2020
AND month = 7
AND day = 28
AND street = "Chamberlin Street";

-- One result returned: ID 295. Description as follows: "Theft of the CS50 duck took place at 10:15am at the Chamberlin Street courthouse.
-- Interviews were conducted today with three witnesses who were present at the time — each of their interview transcripts mentions the courthouse."
-- Search interviews table with date of crime and "courthouse" comments.

SELECT id, name, transcript FROM interviews
WHERE year = 2020
AND month = 7
AND day = 28
AND transcript LIKE "%courthouse%";

-- 3 results returned: Ruth, Eugene, Raymond.
-- 161 | Ruth |  Sometime within ten minutes of the theft, I saw the thief get into a car in the courthouse parking lot and drive away.
-- If you have security footage from the courthouse parking lot, you might want to look for cars that left the parking lot in that time frame.
-- 162 | Eugene | I don't know the thief's name, but it was someone I recognized. Earlier this morning, before I arrived at the courthouse,
-- I was walking by the ATM on Fifer Street and saw the thief there withdrawing some money.
-- 163 | Raymond | As the thief was leaving the courthouse, they called someone who talked to them for less than a minute. In the call,
-- I heard the thief say that they were planning to take the earliest flight out of Fiftyville tomorrow.
-- The thief then asked the person on the other end of the phone to purchase the flight ticket.

-- Follow up on Ruth's tip and look at security footage from courthouse parking lot during crime time 10:15am - 10:25am.
SELECT id, hour, minute, activity, license_plate
FROM courthouse_security_logs
WHERE year = 2020
AND month = 7
AND day = 28
AND HOUR = 10
AND MINUTE BETWEEN 15 AND 25;

-- Results:
-- id | hour | minute | activity | license_plate
-- 260 | 10 | 16 | exit | 5P2BI95
-- 261 | 10 | 18 | exit | 94KL13X
-- 262 | 10 | 18 | exit | 6P58WS2
-- 263 | 10 | 19 | exit | 4328GD8
-- 264 | 10 | 20 | exit | G412CB7
-- 265 | 10 | 21 | exit | L93JTIZ
-- 266 | 10 | 23 | exit | 322W7JE
-- 267 | 10 | 23 | exit | 0NTHK55

-- Follow up on Eugene's tip regarding ATM on Fifer St first before digging into each license for possibility of narrowing list.
SELECT id, account_number, atm_location, transaction_type, amount
FROM atm_transactions
WHERE year = 2020
AND month = 7
AND day = 28
AND atm_location LIKE "%Fifer%"
AND transaction_type LIKE "%withdraw%";

-- Results:
-- id | account_number | atm_location | transaction_type | amount
-- 246 | 28500762 | Fifer Street | withdraw | 48
-- 264 | 28296815 | Fifer Street | withdraw | 20
-- 266 | 76054385 | Fifer Street | withdraw | 60
-- 267 | 49610011 | Fifer Street | withdraw | 50
-- 269 | 16153065 | Fifer Street | withdraw | 80
-- 288 | 25506511 | Fifer Street | withdraw | 20
-- 313 | 81061156 | Fifer Street | withdraw | 30
-- 336 | 26013199 | Fifer Street | withdraw | 35

-- Follow up on Raymond's tip to see if I can narrow the list of suspects before pull all of the above accounts and license plates. Look into the call.
-- Not sure how duration is incremented so just pull by date for now.
SELECT id, caller, receiver, duration
FROM phone_calls
WHERE year = 2020
AND month = 7
AND day = 28;

-- Based on the results above, I'm assuming that duration is storing the number of minutes. So then filter out by duration < 60 to narrow list of calls.
SELECT id, caller, receiver, duration
FROM phone_calls
WHERE year = 2020
AND month = 7
AND day = 28
AND duration < 60;

-- Results:
-- id | caller | receiver | duration
-- 221 | (130) 555-0289 | (996) 555-8899 | 51
-- 224 | (499) 555-9472 | (892) 555-8872 | 36
-- 233 | (367) 555-5533 | (375) 555-8161 | 45
-- 251 | (499) 555-9472 | (717) 555-1342 | 50
-- 254 | (286) 555-6063 | (676) 555-6554 | 43
-- 255 | (770) 555-1861 | (725) 555-3243 | 49
-- 261 | (031) 555-6622 | (910) 555-3251 | 38
-- 279 | (826) 555-1652 | (066) 555-9701 | 55
-- 281 | (338) 555-6650 | (704) 555-2131 | 54

-- Still too many numbers to look through. Follow up on latter half of Raymond's tip regarding earliest flight out of Fiftyville the day after crime.
-- From looking at .schema, flights table does not have text of origin hence I need to retrieve aiport id of Fiftyville first.
SELECT * FROM airports
WHERE city = "Fiftyville";
-- Results:
-- id | abbreviation | full_name | city
-- 8 | CSF | Fiftyville Regional Airport | Fiftyville

SELECT id, origin_airport_id, destination_airport_id, hour, minute
FROM flights
WHERE origin_airport_id = 8
AND year = 2020
AND month = 7
AND day = 29;

-- Results:
-- id | origin_airport_id | destination_airport_id | hour | minute
-- 18 | 8 | 6 | 16 | 0
-- 23 | 8 | 11 | 12 | 15
-- 36 | 8 | 4 | 8 | 20
-- 43 | 8 | 1 | 9 | 30
-- 53 | 8 | 9 | 15 | 20
-- Assuming that this is in military time, then flight 36, destination airport 4, is the most promising as it's the earliest flight out.

-- Find destination city
SELECT * FROM airports
WHERE id = 4;

-- This returns Heathrow, London. Now search for list of passengers on flight.
-- Passengers would be a huge list. People table however has license plate and phone numbers. I should be able to search with the pulled plates and phones above.
-- Instead of writing long SQL statement to find matching license plate, phone number, I'm going to store the above searches into temp variable with common table expressions.
-- Tip from doctoral student and ref: https://www.sqlshack.com/ctes-in-sql-server-querying-common-table-expressions/, https://www.sqlservertutorial.net/sql-server-basics/sql-server-cte/

WITH pulled_phones AS (
SELECT caller, receiver
FROM phone_calls
WHERE year = 2020
AND month = 7
AND day = 28
AND duration < 60),

pulled_plates AS (
SELECT license_plate
FROM courthouse_security_logs
WHERE year = 2020
AND month = 7
AND day = 28
AND HOUR = 10
AND MINUTE BETWEEN 15 AND 25)

SELECT name, passport_number, license_plate
FROM people
WHERE license_plate IN (SELECT license_plate FROM pulled_plates)
AND phone_number IN (SELECT caller FROM pulled_phones);

-- The above search returns a list of four individuals with license plates that matched the suspected cars AND that called during the crime time.
-- Results:
-- name | passport_number | license_plate
-- Roger | 1695452385 | G412CB7
-- Russell | 3592750733 | 322W7JE
-- Evelyn | 8294398571 | 0NTHK55
-- Ernest | 5773159633 | 94KL13X

--  Build on above query, now look among those individuals that also flew on flight 36

WITH pulled_phones AS (
SELECT caller, receiver
FROM phone_calls
WHERE year = 2020
AND month = 7
AND day = 28
AND duration < 60),

pulled_plates AS (
SELECT license_plate
FROM courthouse_security_logs
WHERE year = 2020
AND month = 7
AND day = 28
AND HOUR = 10
AND MINUTE BETWEEN 15 AND 25)

SELECT passengers.passport_number
FROM passengers
WHERE flight_id = 36
AND passport_number IN (SELECT passport_number FROM people WHERE license_plate IN (SELECT license_plate FROM pulled_plates)
AND phone_number IN (SELECT caller FROM pulled_phones));

-- returned 3 passport numbers of individuals that took the earliest flight out of Fiftyville the day after the crime
-- 1695452385
-- 5773159633
-- 8294398571

SELECT id, name
FROM people
WHERE passport_number=1695452385 OR passport_number=5773159633 OR passport_number=8294398571;

-- Results:
-- id | name
-- 398010 | Roger
-- 560886 | Evelyn
-- 686048 | Ernest

-- To narrow list, see which one of these three individuals withdrew from the bank account. Here, I again use CTE as I can just copy and paste my query above)

WITH pulled_bank AS (
SELECT id, account_number, atm_location, transaction_type, amount
FROM atm_transactions
WHERE year = 2020
AND month = 7
AND day = 28
AND atm_location LIKE "%Fifer%"
AND transaction_type LIKE "%withdraw%")

SELECT * FROM bank_accounts
WHERE bank_accounts.account_number IN (SELECT account_number FROM pulled_bank)
AND person_id IN (SELECT id FROM people WHERE passport_number=1695452385 OR passport_number=5773159633 OR passport_number=8294398571);

-- RESULTS:
-- account_number | person_id | creation_year
-- 49610011 | 686048 | 2010
-- Now that I have the person_id, Let me see his snapshot.

SELECT * FROM people
WHERE id = 686048;

-- RESULTS:
-- id | name | phone_number | passport_number | license_plate
-- 686048 | Ernest | (367) 555-5533 | 5773159633 | 94KL13X

-- Now to get the information of his accomplice, I'll use subqueries to cross check Ernest's phone number in the phone call query from earlier before to find the receiver's number and then identity.

SELECT * FROM people
WHERE phone_number IN
(SELECT receiver FROM phone_calls WHERE caller IN (SELECT phone_number FROM people WHERE id = 686048)
AND year = 2020
AND month = 7
AND day = 28
AND duration < 60);

-- According to this query, Berthold is probably the accomplice.