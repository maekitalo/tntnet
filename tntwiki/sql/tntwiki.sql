create table users
(
  id int not null primary key,
  name text not null,
  passwd text   -- md5 salted passwd (first 2 bytes are salt; rest is md5(salt + passwd) )
);

create table pages
(
  id int not null primary key,
  title text not null,
  ts timestamp not null,
  data text,        -- if null, page is deleted
  userid int,

  foreign key (userid) references users(id)
);

create index ix_pages_title_ts
  on pages(title, ts);
