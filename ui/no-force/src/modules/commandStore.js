//Store for sending commands through the dataSocket


const commandStore = {
    state: () => ({
        dataSocket: null,
        servoPosition: 0.00,

       }),
       mutations:{
           SET_DATA_SOCKET(state, socket){
                state.dataSocket = socket;
            },
            SET_POSITION(state, pos){
                state.servoPosition = pos;
                state.dataSocket.send(JSON.stringify({
                    set: "position",
                    to: pos
                }));
            },
            SET_READ(state){
                state.dataSocket.send(JSON.stringify({
                    set: "mode",
                    to: "read"
                }));
            },
            SET_MOVE(state){
                state.dataSocket.send(JSON.stringify({
                    set: "mode",
                    to: "move"
                }));
            },
            SET_STANDBY(state){
                state.dataSocket.send(JSON.stringify({
                    set: "mode",
                    to: "standby"
                }));
            },
            SET_ZERO(state){
                state.dataSocket.send(JSON.stringify({
                    set: "mode",
                    to: "zero"
                }));
                state.servoPosition = 0.00;
            },
            SET_TARE(state){
                state.dataSocket.send(JSON.stringify({
                    set: "mode",
                    to: "tare"
                }));
            },
            SET_TARE_LOAD(state){
                state.dataSocket.send(JSON.stringify({
                    set: "mode",
                    to: "tare_load"
                }));
            },
            
            
            
            

       },
       actions:{
        setDataSocket(context, socket){
            context.commit("SET_DATA_SOCKET", socket);
          },

        async commitCommand(context, command){
            //commits the command instantly
            if(command.parameter){
                context.commit(command.mutation, command.parameter);
            } else{
                context.commit(command.mutation);
            }
            
            //but then waits 100ms until returning a promise
            let promise = new Promise(
                (resolve) => {setTimeout(() => {resolve('done');}, 100)}
            );
            console.log(await promise);
            
        },
        async setPosition(context, pos){
            await context.dispatch('commitCommand', {mutation: 'SET_POSITION', parameter: pos});
            
        },
        async standby(context){
            await context.dispatch('commitCommand', {mutation: 'SET_STANDBY'});
        },
        async read(context){
            await context.dispatch('commitCommand', {mutation: 'SET_READ'});
        },
        async move(context){
            await context.dispatch('commitCommand', {mutation: 'SET_MOVE'});
        },
        async zero(context){
            await context.dispatch('commitCommand', {mutation: 'SET_ZERO'});
        },
        async tare(context){
            await context.dispatch('commitCommand', {mutation: 'SET_TARE'});
            // await context.dispatch('commitCommand', {mutation: 'SET_CURRENT', parameter: {load_cell: 0.0, gauge_1: 0.0, gauge_2: 0.0, gauge_3: 0.0, gauge_4: 0.0, gauge_5: 0.0, gauge_6: 0.0}});
        },
        async tareLoad(context){
            await context.dispatch('commitCommand', {mutation: 'SET_TARE_LOAD'});
        },
        async setPositionAndMove(context, position){
            await context.dispatch('setPosition', position);
            await context.dispatch('move');
        },
        async reset(context){
            await context.dispatch('zero');
        },
        
       },
       getters:{
        getDataSocket(state){
            return state.dataSocket;
          },
          getServoPosition(state){
            return state.servoPosition;
        }
          
         
       },  
  
  }

  export default commandStore;