const config = {

    //Configure the following for your user interface
    remote_lab_ui: 'truss-2.0',
    hardware_code_prefix: 'trus',   //the short code for hardware that is used for each experiment code, e.g. spin in spin30
    click_logs_on: true,
    mouse_enter_logs_on: false,
  
    //For generating graphs in the analytics system, you need to map the appropriate buttons (that send hardware commands to the lab) with the 
    // names of the nodes that will appear on the graph (or a coded name)
    graph_nodes: {
      "load-decrement-10": "decrease_load",
      "load-decrement-5": "decrease_load",
      "load-decrement-1": "decrease_load",
      "load-increment-10": "increase_load",
      "load-increment-5": "increase_load",
      "load-increment-1": "increase_load",
      "load-input": "change_load",
      "load-control-reset": "reset",
      "load-control-tare-gauges": "tare_gauges",
      "load-control-tare-load-cell": "tare_load",
      "snapshot-button": "record"
    },
    //Add any hotkeys that you want to log
    hotkeys: [
    
    ],
    // If you include an object below, it will be sent on an interval
    interval_logs: [
      //{"interval": 5000, "payload": {"msg": "5 second health check", 'state': 6}},
      // {"interval": 30000, "payload": {"msg": "30 second health check"}}
    ],

    
    //If you are configuring for your user interface, leave everything below as is:
    version: import.meta.env.VITE_APP_VERSION,
    logging_path: '/addLog',
    definitions_path: '/verbs',
}

export default config;