#!/bin/bash

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

    BODY=$( printf '{ firstname: "%s", lastname: "%s", phone: "%s" }' $FIRSTNAME $LASTNAME $PHONE )
    httpRequest POST /person "$BODY"
}

makeFriend () {
    local PERSONID=$1
    local FRIENDID=$2
    local BODY=$( printf '{ personId: %d, friendId: %d }' $PERSONID $FRIENDID )
    httpRequest POST /friend "$BODY"
}

HOST=localhost
PORT=8000

echo "+++ add a person"
PERSONID=$(addPerson Tommi Mäkitalo 1234)

echo "+++ add another person"
FRIENDID=$(addPerson Peter Conrad 654)

echo "person id=$PERSONID friend id=$FRIENDID"

ID=$(makeFriend $PERSONID $FRIENDID)

echo "get friends"
httpRequest GET /friend/$PERSONID

httpRequest DELETE /friend/$ID >/dev/null
httpRequest DELETE /person/$FRIENDID >/dev/null
httpRequest DELETE /person/$PERSONID >/dev/null
