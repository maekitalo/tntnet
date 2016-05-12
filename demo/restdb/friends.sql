create table person (
    id integer not null primary key autoincrement,
    firstname text not null,
    lastname text not null,
    phone text not null
);

create table friend (
    id int person not null references person(id),
    friend int person not null references person(id),
    primary key (id, friend)
);
