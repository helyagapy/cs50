-- SQL query to list the names of all people who starred in a movie in which Kevin Bacon also starred
SELECT people.name
FROM people
JOIN stars
ON people.id = stars.person_id
WHERE people.id IN
(SELECT stars.person_id WHERE stars.movie_id IN
(SELECT stars.movie_id FROM stars JOIN people ON stars.person_id = people.id WHERE people.name = "Kevin Bacon" AND people.birth = 1958))
AND people.name != "Kevin Bacon";