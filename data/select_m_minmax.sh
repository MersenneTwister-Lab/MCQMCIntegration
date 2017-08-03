sqlite3 digitalnet.sqlite3 <<EOF
select dimr, min(dimf2), max(dimf2) from digitalnet
  where id = $1
  group by dimr;
EOF
