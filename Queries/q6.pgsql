copy(
select
case
when AX.mname!='NULL' and AX.lname !='NULL' then  concat(AX.fname,' ',AX.mname,' ',AX.lname)
when AX.mname='NULL' and AX.lname='NULL' then AX.fname
when AX.lname!='NULL' and AX.mname='NULL' then concat(AX.fname,' ',AX.lname) end X_name,

case
when AY.mname!='NULL' and AY.lname !='NULL' then  concat(AY.fname,' ',AY.mname,' ',AY.lname)
when AY.mname='NULL' and AY.lname='NULL' then AY.fname
when AY.lname!='NULL' and AY.mname='NULL' then concat(AY.fname,' ',AY.lname) end Y_name,

case
when AZ.mname!='NULL' and AZ.lname !='NULL' then  concat(AZ.fname,' ',AZ.mname,' ',AZ.lname)
when AZ.mname='NULL' and AZ.lname='NULL' then AZ.fname
when AZ.lname!='NULL' and AZ.mname='NULL' then concat(AZ.fname,' ',AZ.lname) end Z_name,
count(*) as count
from
(
select least(X_au_id,Y_au_id,Z_au_id) as newX_au_id,
CASE least(X_au_id,Y_au_id,Z_au_id) WHEN X_au_id THEN least(Y_au_id,Z_au_id)
                                  WHEN Y_au_id THEN least(X_au_id,Z_au_id)
                                  WHEN Z_au_id THEN least(X_au_id,Y_au_id)
                                  END AS newY_au_id,
greatest(X_au_id,Y_au_id,Z_au_id) as newZ_au_id
from
(
select ABX.author_id as X_au_id,ABY.author_id as Y_au_id,ABZ.author_id as Z_au_id
from Reference Z
left join Reference Y
on Z.reference_paper_id = Y.paper_id and Z.paper_id > Y.paper_id
left join Reference X
on Y.reference_paper_id = X.paper_id and Y.paper_id > X.paper_id
left join AuthoredBy ABX
on ABX.paper_id = X.paper_id
left join AuthoredBy ABY
on ABY.paper_id = Y.paper_id
left join AuthoredBy ABZ
on ABZ.paper_id = Z.paper_id
where X.reference_paper_id = Z.paper_id and ABX.author_id != ABY.author_id and ABY.author_id!=ABZ.author_id and ABZ.author_id != ABX.author_id
group by ABX.author_id,ABY.author_id,ABZ.author_id
) alias1
) alias2 left join author AX
on newX_au_id = AX.author_id
left join author AY
on newY_au_id = AY.author_id
left join author AZ
on newZ_au_id = AZ.author_id
group by AX.fname,AX.mname,AX.lname,
AY.fname,AY.mname,AY.lname,AZ.fname,AZ.mname,AZ.lname
order by count DESC)
to '/tmp/q6.csv' with csv delimiter '|' header;
