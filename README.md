Smart-meter-P1
==============

Readout of Dutch smart meter P1-port using Arduino and logging to EmonCMS

What do you need?
-----------------

* Any Arduino, although a Duemilanove might not work due to high memory usage(>16k)
* A RJ11 cable (check if it has 4 wires!)

Optionally:
-----------

* Arduino ethernet shield (Wiznet based)
* A computer or server running EmonCMS (www.openenergymonitor.org)

How does it work?
-----------------

During the setup process after powering the Arduino, 5volt is enabled to pin4. This in turn enables the logging from the Smart Meter. Once the Smart Meter has ended sending the telegram, the Arduino (tries to) decode it.

Will it work for me?
--------------------

I don't have a gasmeter, so if you want to read that from your Smart Meter, you'll have to extend the code. Just follow the output from the Smart Meter, which you can also output to the serial window (uncomment 'Serial.println(inputString);'). Also, you can decode other parts of the telegram, which I wasn't interested in (such as current tariff). If you want to send your data to Cosm, just change the httpRequest function to your needs.

Are there any issues?
---------------------

An Arduino is really not suitable for large string operations, but it kinda works in my situation. I'm suspecting that larger P1 telegram messages might not fully work. Also, the first telegram it receives should really be ignored, although that could be a timing issue.

Additions or fixes to my code are more than welcome. Just know that this is my third Arduino project and that most code has been copied from much smarter and more capable developers.