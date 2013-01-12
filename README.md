uniqidserver
============

A unique ID server. Runs a TCP server. Connect to that server and it will send you an ID and immediately close the connection.

    Usage: uniqidserver <port> <machine_id>

The machine ID should be a number between 0 and 255. The number is used when generating IDs. This enables you to run multiple ID servers and ensures that IDs created on different machines will never clash.

IDs are as follows:

    aaaabbccccddd

Where:

    aaaa = first four chars of the secs since epoch in hex
    bb   = the machine ID in hex
    cccc = the rest of the secs since epoch in hex
    ddd  = the ms part of time time since epoch in hex

This gives us IDs that are roughly ordered by time, even across servers, that don't have a 00 at the end of every ID if only running one ID server (always looked odd to me), and is horizontally scalable across up to 256 servers. To add more servers simply increase the machine ID from one to two bytes.

If the machine ID is not provided, is -1, or fails to convert to an integer between 0 and 255, the bb part above will be excluded.

License
-------

No license; no warranty; no nothing. I haven't deployed this to any great capacity yet, so feel free to use it, abuse it, ignore it, or print it out and eat it. Just don't come running to me when it explodes or you throw it all up again :)

Contributing
------------

Contributions are welcome. Simply fork it, change it, and send a pull request.

TODO
----

* Add a mode so it can correctly handle HTTP requests so things like PHP's file_get_contents can be used to easily access it.

* Add a mode such that a client connects and then sends a command to get a new ID. This would be for long-running scripts that need to generate a lot of IDs where keeping a connection open would be more efficient than creating one for each ID needed.

Who be me, and why be this?
---------------------------

Me be Stuart Dallas, and my company is 3ft9 Ltd. I wrote this daemon to solve the problem of creating unique IDs across multiple servers for use with NoSQL databases. I'm aware that there are other established projects that also solve this problem, but I decided to write my own.

Stuart Dallas<br />
3ft9 Ltd<br />
http://3ft9.com/
