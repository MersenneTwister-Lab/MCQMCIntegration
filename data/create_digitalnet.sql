drop table digitalnet_id;
create table digitalnet_id (
        id int,
	netname text,
        longname text,
        primary key (id)
        );

drop table digital_tb;
create table digital_tb (
	id int,
        bitsize int,
        dimr int,
        dimf2 int,
        wafom real,
        tvalue int,
        data text,
        primary key (id, bitsize, dimr, dimf2)
        );

/* view */
drop view digitalnet;
create view digitalnet as select
    name as netname, i.id, bitsize, dimr, dimf2, wafom, tvalue, data
    from digital_tb t join digitalnet_id i
    where t.id = i.id;
