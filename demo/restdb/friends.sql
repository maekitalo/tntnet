create table person (
    id integer not null primary key autoincrement,
    firstname text not null,
    lastname text not null,
    phone text not null
);

create table friend (
    id integer not null primary key autoincrement,
    personid integer person not null references person(id),
    friendid integer person not null references person(id)
);

create unique index friendids_ix on friend(personid, friendid);
