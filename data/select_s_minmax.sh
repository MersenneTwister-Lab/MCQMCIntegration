sqlite3 digitalnet.sqlite3 <<EOF
select min(dimr), max(dimr) from digitalnet where id = $1;
EOF
