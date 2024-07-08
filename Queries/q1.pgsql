create temporary table paper_venue as
select index, title, year, V1.location as location, citedby_index, citedby_title, citedby_year, V2.location as citedby_location from
( select P1.index as index, P1.title as title, P1.year as year, P1.venue_id as venue_id, paper_id as citedby_index, P2.title as citedby_title,P2.year as citedby_year,P2.venue_id as citedby_venue_id
 from paper P1 inner join reference on index = reference_paper_id
inner join paper P2 on P2.index = paper_id)
as paper_reference
inner join venue V1 on paper_reference.venue_id = V1.venue_id
inner join venue V2 on paper_reference.citedby_venue_id = V2.venue_id order by index;

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
select distinct index, title, year, location, A1.authors as authors, citedby_index, citedby_title, citedby_year, citedby_location, A2.authors as citedby_authors
from paper_venue left join paper_author A1 on paper_venue.index = A1.paper_id
left join paper_author A2 on paper_venue.citedby_index = A2.paper_id
order by index
) to '/tmp/q1.csv' with csv delimiter '|' header;
