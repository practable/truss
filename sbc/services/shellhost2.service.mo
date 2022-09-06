[Unit]
Description=shell ssh host service
After=network.target

[Service]
Restart=on-failure
Environment=SHELLHOST_LOCALPORT=22
Environment=SHELLHOST_RELAYSESSION={{SESSION}}
Environment=SHELLHOST_TOKEN={{TOKEN}}
Environment=SHELLHOST_DEVELOPMENT=true
ExecStart=/usr/local/bin/shell-relay host

[Install]
WantedBy=multi-user.target
