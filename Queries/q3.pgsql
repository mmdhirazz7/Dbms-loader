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

create temporary table paper_venue
as select paper.index , venue.location ,year ,title
from paper inner join venue on paper.venue_id = venue.venue_id;


create temporary table paperwholeinfo as
select paper_venue.index , authors , location , year , title
from paper_venue inner join paper_author
on paper_venue.index = paper_author.paper_id;

copy(
select infox.index as x, infox.title as x_title, infox.authors as x_authors, infox.year as x_year, infox.location as x_location,
infoz.index as z, infoz.title as z_title, infoz.authors as z_authors, infoz.year as z_year, infoz.location as z_location
from (
select tb1.reference_paper_id as x, tb2.paper_id as z
from reference as tb1 inner join reference as tb2 on tb1.paper_id = tb2.reference_paper_id and tb1.reference_paper_id != tb2.paper_id
) as solution_table inner join paperwholeinfo as infox on solution_table.x = infox.index
inner join paperwholeinfo as infoz on solution_table.z = infoz.index
order by x)
to '/tmp/q3.csv' with csv delimiter '|' header;
