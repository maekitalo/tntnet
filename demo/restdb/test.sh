#!/bin/bash

urlencode () {
    perl -MURI::Escape -e 'print uri_escape($ARGV[0]);' $1
}

addPerson () {
    local FIRSTNAME=$1
    local LASTNAME=$2
    local PHONE=$3
    curl http://$HOST:$PORT/person -d '{firstname:"'$FIRSTNAME'",lastname:"'$LASTNAME'",phone:"'$PHONE'"}'
}

updatePerson () {
    local ID=$1
    local FIRSTNAME=$2
    local LASTNAME=$3
    local PHONE=$4

    BODY='{firstname:"'$FIRSTNAME'",lastname:"'$LASTNAME'",phone:"'$PHONE'"}'
    # ${#BODY} do not work here because it returns the length in characters instead of bytes
    # ä is 1 character but 2 bytes since it is utf-8 encoded
    LENGTH=$(echo -n $BODY|wc -c)
    echo -ne "PUT /person/$ID HTTP/1.1\r\nConnection:close\r\nContent-Length: $LENGTH\r\n\r\n$BODY"|nc $HOST $PORT >/dev/null
}

deletePerson () {
    local ID=$1
    echo -ne 'DELETE /person/'$ID' HTTP/1.1\r\nConnection:close\r\n\r\n'|nc $HOST $PORT
}

getPersonById () {
    local ID=$1

    curl http://$HOST:$PORT/person/$ID
}

getPersonByName () {
    local FIRSTNAME=$(urlencode $1)
    local LASTNAME=$(urlencode $2)

    curl http://$HOST:$PORT/person/$LASTNAME/$FIRSTNAME
}

HOST=localhost
PORT=8000

echo "+++ add a person"
ID=$(addPerson Tommi Mäkitalo 1234)

echo "+++ read a person by id $ID"
P=$(curl http://$HOST:$PORT/person/$ID)
echo Person: $P

echo "+++ update the phone number to 1235"
updatePerson $ID Tommi Mäkitalo 12345

echo "+++ read a person by name (name must be url encoded)"
P=$(getPersonByName Tommi Mäkitalo)
echo Person: $P

echo "+++ delete person $ID"
deletePerson $ID
