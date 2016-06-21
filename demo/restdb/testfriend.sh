#!/bin/bash

HOST=localhost
PORT=8000

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

    BODY=$( printf '{ firstname: "%s", lastname: "%s", phone: "%s" }' $FIRSTNAME $LASTNAME $PHONE )
    httpRequest POST person "$BODY"
}

makeFriend () {
    local PERSONID=$1
    local FRIENDID=$2
    local BODY=$( printf '{ personId: %d, friendId: %d }' $PERSONID $FRIENDID )

    httpRequest POST friend "$BODY"
}

echo "+++ add a person"
PERSONID=$(addPerson Donald Duck 1234)
echo personid=$PERSONID

echo "+++ add another person"
FRIENDID=$(addPerson Dagobert Duck 654)
FRIEND2ID=$(addPerson Mickey Mouse 654)

echo "person id=$PERSONID friend id=$FRIENDID id=$FRIEND2ID"

ID=$(makeFriend $PERSONID $FRIENDID)
ID=$(makeFriend $PERSONID $FRIEND2ID)

echo "get friends"
httpRequest GET friend/$PERSONID

httpRequest DELETE friend/$ID >/dev/null
httpRequest DELETE person/$PERSONID >/dev/null
httpRequest DELETE person/$FRIENDID >/dev/null
httpRequest DELETE person/$FRIEND2ID >/dev/null
