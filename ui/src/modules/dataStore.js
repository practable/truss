//Store for DATA relating to the experiment itself. Data being read from the remote lab hardware or being displayed within the interface.


const dataStore = {
    state: () => ({
         current: {load_cell: 2.0, gauge_1: 0.1, gauge_2: 0.2, gauge_3: 0.3, gauge_4: 0.4, gauge_5: 0.5, gauge_6: 0.6},   //{load_cell: 2.0, gauge_1: 0.5, gauge_2: 0.5, gauge_3: 0.5, gauge_4: 0.5, gauge_5: 0.5, gauge_6: 0.5}
         history:[],    //array of objects of form of current above   
         sm_state: 0,      //the statemachine state of the firmware: STATE_STANDBY = 0, STATE_READ = 1, STATE_MOVE = 2, STATE_ZERO = 3, STATE_TARE = 4, STATE_GAUGE_RESET = 5 
         initial_position: 0,      //the initial position of the stepper motor
         x_axis_variable: 'gauge_1',
         y_axis_variable: 'gauge_2', 
       }),
       mutations:{
         SET_CURRENT(state, data){
            state.current = data;
         },
         ADD_TO_HISTORY(state, data){
            state.history.push(data);
         },
         DELETE_HISTORY (state, dataId) {
            state.history.splice(dataId, 1);
         },
         CLEAR_ALL_HISTORY(state){
            state.history = [];
            //state.data.length = 0; //all references to data are cleared
         },
         SET_SM_STATE(state, sm_state){
            state.sm_state = sm_state;
         },
         SET_INITIAL_POSITION(state, pos){
            state.initial_position = pos;
         },
         SET_X_AXIS(state, variable){
            state.x_axis_variable = variable;
         },
         SET_Y_AXIS(state, variable){
            state.y_axis_variable = variable;
         }
         
       },
       actions:{
         setCurrent(context, data){
            context.commit('SET_CURRENT', data)
         },
         addToHistory(context, data){
            context.commit("ADD_TO_HISTORY", data);
         },
         deleteHistory(context, dataId){
            context.commit('DELETE_HISTORY', dataId);
         },
         clearAllHistory(context){
            context.commit('CLEAR_ALL_HISTORY');
         },
         setSmState(context, sm_state){
            context.commit('SET_SM_STATE', sm_state);
         },
         setInitialPosition(context, pos){
            context.commit('SET_INITIAL_POSITION', pos);
         },
         setXAxis(context, variable){
            context.commit('SET_X_AXIS', variable);
         },
         setYAxis(context, variable){
            context.commit('SET_Y_AXIS', variable);
         }

       },
       getters:{
         getCurrent(state){
            return state.current;
         },
         getHistory(state){
            return state.history;
         },
         getSmState(state){
            return state.sm_state;
         },
         getInitialPosition(state){
            return state.initial_position;
         },
         getXAxisVariable(state){
            return state.x_axis_variable;
         },
         getYAxisVariable(state){
            return state.y_axis_variable;
         },
         getXDataLatest(state){
            return state.current[state.x_axis_variable];
         },
         getXDataAll(state){
            let data = [];
            for(let i=0;i<state.history.length;i++){
               data[i] = state.history[i][state.x_axis_variable];
            }

            return data;
         },
         getYDataLatest(state){
            return state.current[state.y_axis_variable];
         },
         getYDataAll(state){
            let data = [];
            for(let i=0;i<state.history.length;i++){
               data[i] = state.history[i][state.y_axis_variable];
            }

            return data;
         },
         getNumData(state){
            return state.history.length;
         }
          
         
       },  
  
  }

  export default dataStore;
