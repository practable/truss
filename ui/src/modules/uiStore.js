//Store for variables that are common amongst multiple UI components. 


const uiStore = {
    state: () => ({
        isDraggable: true,
        consent_given: false,
        show_force: false,       //should force values be displayed on the UI
        show_theory: false,      //should the theoretical values component be available on the UI
        input_disabled: false,
        hard_limit: false,
       }),
       mutations:{
         SET_DRAGGABLE(state, draggable){
            state.isDraggable = draggable;
         },
         SET_CONSENT(state, consent){
            state.consent_given = consent;
         },
         SET_INPUT_DISABLED(state, set){
            console.log('INPUT DISABLED ', set);
            state.input_disabled = set;
        },
        SET_HARD_LIMIT(state, set){
            console.log('HARD LIMIT ', set);
            state.hard_limit = set;
        }

       },
       actions:{
         setDraggable(context, draggable){
             context.commit('SET_DRAGGABLE', draggable);
         },
         setConsent(context, consent){
            context.commit('SET_CONSENT', consent);
         },
         disableInput(context, set){
            console.log('disable input');
            context.commit('SET_INPUT_DISABLED', set);
        },
        setHardLimit(context, set){
            console.log('set hard limit', set);
            context.commit('SET_HARD_LIMIT', set);
        }

       },
       getters:{
         getDraggable(state){
             return state.isDraggable;
         },
         getConsent(state){
            return state.consent_given;
         },
         getShowForce(state){
            return state.show_force;
         },
         getShowTheory(state){
            return state.show_theory;
         },
         getInputDisabled(state){
            return state.input_disabled;
         },
         getHardLimit(state){
            return state.hard_limit;
         }
          
         
       },  
  
  }

  export default uiStore;
