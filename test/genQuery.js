#!/usr/bin/node

var jsdom = require('jsdom');
const { JSDOM } = jsdom;
const { window } = new JSDOM();
const { document } = (new JSDOM('')).window;
global.document = document;

var $ = require('jquery')(window);

process.stdout.write($.param({
    simple: "Hi",
    numbers: [ 1, 4, 5 ],
    emptyArray: [],
    sub: {
        i: 42,
        a: [ "Hi", "there" ]
    }
}));
