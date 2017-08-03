/* view */
drop view digitalnet;
create view digitalnet as select
    netname, i.id, bitsize, dimr, dimf2, wafom, tvalue, data
    from digital_tb t join digitalnet_id i
    where t.id = i.id;
