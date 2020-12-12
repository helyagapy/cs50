-- SQL query to list the titles of all movies in which both Johnny Depp and Helena Bonham Carter starred.
SELECT movies.title FROM movies
JOIN stars
ON movies.id = stars.movie_id
JOIN people
ON stars.person_id = people.id
WHERE movies.id IN (SELECT stars.movie_id WHERE people.name IN ("Johnny Depp" , "Helena Bonham Carter"))
GROUP BY stars.movie_id
HAVING COUNT(stars.movie_id) > 1;