create temporary table paper_author as
select paper_id, string_agg(author_name,', ' order by priority asc) authors
from authoredby inner join
(select author_id,
case
when mname!='NULL' and lname !='NULL' then  concat(fname,' ',mname,' ',lname)
when mname='NULL' and lname='NULL' then fname
when lname!='NULL' and mname='NULL' then concat(fname,' ',lname) end author_name
from author
) as author_name
on  author_name.author_id = authoredby.author_id group by paper_id;

copy(
select reference.reference_paper_id as id,paper.title,paper_author.authors,paper.venue_id,venue.location,paper.year,count(reference.reference_paper_id) as cnt
from reference
left join paper
on reference.reference_paper_id = paper.index
left join paper_author
on paper.index = paper_author.paper_id
left join venue
on paper.venue_id = Venue.venue_id
Group by reference.reference_paper_id,paper.title,paper_author.authors,paper.venue_id,venue.location,paper.year
ORDER BY cnt DESC
fetch first 20 rows only
)
to '/tmp/q4.csv' with csv delimiter '|' header;


