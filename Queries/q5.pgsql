copy(
select distinct a1.author_id as Author_ID1,
case
when Au1.mname!='NULL' and Au1.lname !='NULL' then  concat(Au1.fname,' ',Au1.mname,' ',Au1.lname)
when Au1.mname='NULL' and Au1.lname='NULL' then Au1.fname
when Au1.lname!='NULL' and Au1.mname='NULL' then concat(Au1.fname,' ',Au1.lname) end Author1_name,

Au2.author_id as Author_ID2,
case
when Au2.mname!='NULL' and Au2.lname !='NULL' then  concat(Au2.fname,' ',Au2.mname,' ',Au2.lname)
when Au2.mname='NULL' and Au2.lname='NULL' then Au2.fname
when Au2.lname!='NULL' and Au2.mname='NULL' then concat(Au2.fname,' ',Au2.lname) end Author2_name

from AuthoredBy a1
left join AuthoredBy a2
on a1.paper_id = a2.paper_id and a1.author_id < a2.author_id
left join AuthoredBy a3
on a3.author_id = a2.author_id and a1.paper_id != a3.paper_id
left join AuthoredBy a4
on a3.paper_id = a4.paper_id and a4.author_id = a1.author_id
left join author Au1
on a1.author_id = Au1.author_id
left join author Au2
on a2.author_id = Au2.author_id
where a2.author_id is not null and a3.paper_id is not null and a4.paper_id is not null
order by a1.author_id
) to '/tmp/q5.csv' with csv delimiter '|' header;
