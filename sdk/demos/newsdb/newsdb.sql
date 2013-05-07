create table article
(
  id integer not null primary key,
  ctime timestamp not null,
  mtime timestamp not null,
  title text not null,
  short_text text not null,
  long_text text not null
);

create index article_ix1 on article (ctime, id);
