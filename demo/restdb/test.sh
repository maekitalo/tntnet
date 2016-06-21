#!/bin/bash

urlencode () {
    perl -MURI::Escape -e 'print uri_escape($ARGV[0]);' $1
}

httpRequest () {
    local METHOD=$1
    local URL=$2
    local BODY=$3

    # execute a http request with the requested method and write the
    # result to STDOUT
    wget -q --method=$METHOD http://$HOST:$PORT/$URL --body-data="$BODY" -O -
}

addPerson () {
    local FIRSTNAME=$1
    local LASTNAME=$2
    local PHONE=$3

    local BODY=$( printf '{ firstname: "%s", lastname: "%s", phone: "%s" }' $FIRSTNAME $LASTNAME $PHONE )
    httpRequest POST person "$BODY"
}

updatePerson () {
    local ID=$1
    local FIRSTNAME=$2
    local LASTNAME=$3
    local PHONE=$4

    local BODY=$( printf '{ firstname: "%s",lastname: "%s",phone: "%s" }' $FIRSTNAME $LASTNAME $PHONE )
    httpRequest PUT person/$ID "$BODY"
}

deletePerson () {
    local ID=$1
    httpRequest DELETE person/$ID
}

getPersonById () {
    local ID=$1
    httpRequest GET person/$ID
}

getPersonByName () {
    local FIRSTNAME=$1
    local LASTNAME=$2
    httpRequest GET person/$(urlencode $LASTNAME)/$(urlencode $FIRSTNAME)
}

searchPerson () {
    local FIRSTNAME=$1
    local LASTNAME=$2
    local PHONE=$3
    httpRequest SEARCH person/$(urlencode $FIRSTNAME)/$(urlencode $LASTNAME)/$(urlencode $PHONE)
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
