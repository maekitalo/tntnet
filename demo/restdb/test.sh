#!/bin/bash

HOST=localhost
PORT=8000

echo "+++ add a person"
ID=$(curl http://$HOST:$PORT/person -d '{firstname:"Tommi",lastname:"Mäkitalo",phone:"1234"}')

echo "+++ read a person by id $ID"
P=$(curl http://$HOST:$PORT/person/$ID)
echo Person: $P

echo "+++ update the phone number to 1235"
# ${#BODY} do not work here because it returns the length in characters instead of bytes
# ä is 1 character but 2 bytes since it is utf-8 encoded
BODY='{firstname:"Tommi",lastname:"Mäkitalo",phone:"1235"}'
LENGTH=$(echo -n $BODY|wc -c)
echo -ne "PUT /person/$ID HTTP/1.1\r\nConnection:close\r\nContent-Length: $LENGTH\r\n\r\n$BODY"|nc $HOST $PORT >/dev/null

echo "+++ read a person by name (name must be url encoded)"
P=$(curl http://$HOST:$PORT/person/M%c3%a4kitalo/Tommi)
echo Person: $P

# delete a person
echo -ne 'DELETE /person/$ID HTTP/1.1\r\nConnection:close\r\n'|nc $HOST $PORT
