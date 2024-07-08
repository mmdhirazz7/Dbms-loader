create temporary table paper_venue as
select index, title, year, V1.location as location, citingto_index, citingto_title, citingto_year, V2.location as citingto_location from
( select P1.index as index, P1.title as title, P1.year as year, P1.venue_id as venue_id,reference_paper_id as citingto_index, P2.title as citingto_title, P2.year as citingto_year, P2.venue_id as citingto_venue_id  
from paper P1 inner join reference on P1.index = paper_id
inner join paper P2 on P2.index = reference_paper_id)
as paper_reference
inner join venue V1 on paper_reference.venue_id = V1.venue_id 
inner join venue V2 on paper_reference.citingto_venue_id = V2.venue_id 
order by index;

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
select distinct index, title, year, location,A1.authors as authors ,citingto_index, citingto_title, citingto_year, citingto_location,A2.authors as citingto_authors 
from paper_venue inner join paper_author A1 on paper_venue.index = A1.paper_id 
inner join paper_author A2 on paper_venue.citingto_index = A2.paper_id
order by index
) to '/tmp/q2.csv' with csv delimiter '|' header;
