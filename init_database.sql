use zz_sql_project

/*
drop table categories
drop table subcategories
drop table users
drop table items
drop table info
drop table roles
drop table warehouse
drop table orders

go
drop trigger tr_delete
*/

-- ok
create table categories(
id int identity(1,1) primary key not null,
name varchar(64) not null
)

-- ok
create table subcategories(
id int identity(1,1) primary key not null,
name varchar(64) not null,
id_category int not null
)

-- ok
create table users(
id int identity(1,1) primary key not null,
login varchar(64) not null,
passwd varchar(64) not null,
email varchar(64) not null,
name varchar(64) not null,
surname varchar(64) not null,
adress varchar(64) not null,
role int not null
)

-- ok
create table roles(
id int primary key not null,
name varchar(64) not null
)

-- ok
create table items(
id int identity(1,1) primary key not null,
name varchar(64) not null,
price int not null,
id_info int not null
)

-- ok
create table info(
id int identity(1,1) primary key not null,
model varchar(64) not null,
producent varchar(64) not null,
id_category int not null,
id_subcategory int not null
)

-- ok
create table orders(
id int identity(1,1) primary key not null,
id_user int not null,
id_item int not null,
adress varchar(64)
)

-- ok
create table warehouse(
id_item int primary key not null,
quantity int not null
)

--------
go
create trigger tr_delete on items
for delete
as
begin
	delete.warehouse where warehouse.id_item = (select id from deleted)
end
--------

insert into roles values(0, 'Free')
insert into roles values(1, 'Normal')
insert into roles values(2, 'Admin')

insert into users values('admin', 'admin', 'admin@admin.com', 'Jan', 'Adminowy', 'Adminowo', 2)

insert into categories values('Gitary')
insert into categories values('Instrumenty dête')

insert into subcategories values('Akustyczne', 1)
insert into subcategories values('Klasyczne', 1)
insert into subcategories values('Elektryczne', 1)
insert into subcategories values('Flety pod³u¿ne', 2)
insert into subcategories values('Flety poprzeczne', 2)
