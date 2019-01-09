This is a little Arduino project of me.

Feel free to use it for whatever you like.

I got multiple arduino nano boards with sensors attached to it. They all report their readings via a 433mhz radio to the collector node (a arduino Uno Board with an Ethernet Shield) which provides all the readings in a [Prometheus](https://prometheus.io) scrapeable way.

I then generate Grafana Dashboards and alerts on top of it.
