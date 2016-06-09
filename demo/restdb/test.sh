#!/bin/bash

urlencode () {
    perl -MURI::Escape -e 'print uri_escape($ARGV[0]);' $1
}

httpRequest () {
    local METHOD=$1
    local URL=$2
    local BODY=$3

    # This function implements a http clinet using nc and awk.
    # echo -e interprets escape sequences like \r and \n and converts them to proper bytes.
    # The awk prints everything after the http headers. Note that a http header is ended by \r\n.
    # Since awk receives each line without linefeed (\n) a single cr (\r) marks
    # the end of headers.

    {
        if [ "$BODY" ]
        then
            # ${#BODY} do not work here because it returns the length in characters instead of bytes
            # ä is 1 character but 2 bytes since it is utf-8 encoded
            local LENGTH=$(echo -n $BODY|wc -c)
            echo -ne "$METHOD $URL HTTP/1.1\r\nConnection:close\r\nContent-Length: $LENGTH\r\n\r\n$BODY"
        else
            echo -ne "$METHOD $URL HTTP/1.1\r\nConnection:close\r\n\r\n"
        fi

    } | nc $HOST $PORT | awk 'BODY { print } $1 == "\r" { BODY=1 }'
}

addPerson () {
    local FIRSTNAME=$1
    local LASTNAME=$2
    local PHONE=$3

    local BODY=$( printf '{ firstname: "%s", lastname: "%s", phone: "%s" }' $FIRSTNAME $LASTNAME $PHONE )
    httpRequest POST /person "$BODY"
}

updatePerson () {
    local ID=$1
    local FIRSTNAME=$2
    local LASTNAME=$3
    local PHONE=$4

    local BODY=$( printf '{ firstname: "%s",lastname: "%s",phone: "%s" }' $FIRSTNAME $LASTNAME $PHONE )
    httpRequest PUT /person/$ID "$BODY"
}

deletePerson () {
    local ID=$1
    httpRequest DELETE /person/$ID
}

getPersonById () {
    local ID=$1
    httpRequest GET /person/$ID
}

getPersonByName () {
    local FIRSTNAME=$1
    local LASTNAME=$2
    httpRequest GET /person/$(urlencode $LASTNAME)/$(urlencode $FIRSTNAME)
}

searchPerson () {
    local FIRSTNAME=$1
    local LASTNAME=$2
    local PHONE=$3
    httpRequest SEARCH /person/$(urlencode $FIRSTNAME)/$(urlencode $LASTNAME)/$(urlencode $PHONE)
}

HOST=localhost
PORT=8000

echo "+++ add a person"
ID=$(addPerson Tommi Mäkitalo 1234)

echo "+++ read a person by id $ID"
P=$(getPersonById $ID)
echo Person: "$P"

echo "+++ update the phone number to 1235"
updatePerson $ID Tommi Mäkitalo 12345

echo "+++ read a person by name"
P=$(getPersonByName Tommi Mäkitalo)
echo Person: "$P"

echo "+++ search person"
searchPerson M m 2

echo "+++ delete person $ID"
deletePerson $ID
