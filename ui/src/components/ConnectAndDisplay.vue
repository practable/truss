<template>
  <div class='container-fluid m-2 background-white border rounded'>

    <div v-if='getSessionExpired' class='col-12'>
        <img id='session-end-image' src='https://assets.practable.io/images/common/thank-you-screen.svg' alt='session ended'>
    </div>

      <div v-else class='col-sm-12' id='overlay'>
            <img id='truss-image' src='../../public/images/truss-mini-image.png' alt='truss-image'>

            <div :class="[{highlighted: (highlighted_index.includes(0))}, {unhighlighted: (!highlighted_index.includes(0))}, 'pt-2']" id='load-cell' :hidden='!getShowForce'>{{getCurrent.load_cell}} N</div>

            <div :class="[{highlighted: (highlighted_index.includes(1))}, {unhighlighted: (!highlighted_index.includes(1))}, 'pt-2']" id='gauge-1'>{{getCurrent.gauge_1}} &mu;&epsilon;</div>

            <div :class="[{highlighted: (highlighted_index.includes(2))}, {unhighlighted: (!highlighted_index.includes(2))}, 'pt-2']" id='gauge-2'>{{getCurrent.gauge_2}} &mu;&epsilon;</div>

            <div :class="[{highlighted: (highlighted_index.includes(3))}, {unhighlighted: (!highlighted_index.includes(3))}, 'pt-2']" id='gauge-3'>{{getCurrent.gauge_3}} &mu;&epsilon;</div>

            <div :class="[{highlighted: (highlighted_index.includes(4))}, {unhighlighted: (!highlighted_index.includes(4))}, 'pt-2']" id='gauge-4'>{{getCurrent.gauge_4}} &mu;&epsilon;</div>

            <div :class="[{highlighted: (highlighted_index.includes(5))}, {unhighlighted: (!highlighted_index.includes(5))}, 'pt-2']" id='gauge-5'>{{getCurrent.gauge_5}} &mu;&epsilon;</div>

            <div :class="[{highlighted: (highlighted_index.includes(6))}, {unhighlighted: (!highlighted_index.includes(6))}, 'pt-2']" id='gauge-6'>{{getCurrent.gauge_6}} &mu;&epsilon;</div>
      </div>

    <toolbar parentCanvasID="" parentComponentName="display" parentDivID="overlay" :showDownload='true' :showPopupHelp="true" :showOptions="false">  
        
        <template v-slot:popup id='display-popup'>
            <div class='row mb-2' id='help-div'>
          
              <h3> Strain gauge Display </h3>
              <p> Values from the 6 strain gauges and load cell are displayed here. The next gauge is updated approximately every second. After changing the load, please 
                allow all values to update.
              </p>
                
            </div>
        </template>

    </toolbar>
    
  </div>
</template>

<script>
import { mapGetters } from 'vuex';
import Toolbar from './elements/Toolbar.vue';

export default {
  name: 'ConnectAndDisplay',
  components: {
    Toolbar,
  },
  props:{
		url: String,
	},
  data(){
    return {
      dataSocket: null,
      highlighted_index: [],
    }
  },
  watch:{
    url(){
			this.connect();
		},
  },
  mounted(){
    //this.connect();
  },
  computed:{
    ...mapGetters([
			'getCurrent',
            'getSessionExpired',
            'getShowForce'
		]),
  },
  methods:{
    getUpdatedIndex(updated, old){
      let updated_values = Object.values(updated);
      let old_values = Object.values(old);
      let updated_indices = []

      for(let i=0;i<updated_values.length;i++)
      {
        if(updated_values[i] != old_values[i])
        {
          updated_indices.push(i);
        }
      }

      return updated_indices;
    },
    connect(){
        let debug = false;
        let _this = this;
        //this.dataSocket = new WebSocket('wss://video.practable.io:443/bi/dpr/pendulum0');
        //console.log(this.url);
        this.dataSocket = new WebSocket(this.url);
        this.$store.dispatch('setDataSocket', this.dataSocket);
        
        this.dataSocket.onopen = () => {
            //console.log(this.$store.getters.getDataSocket);
            console.log('data connection opened');
            _this.$store.dispatch('reset');       //CAN CHOOSE WHETHER TO RESET ON CONNECTION -> REFRESH ETC
        };

        this.dataSocket.onmessage = (event) => {
            try {
                let data = JSON.parse(event.data);      //currently sent as {"load_cell": 45.6, "gauge_1":56.8......, "state": 0, "pos": 2000}
                this.$store.dispatch('setSmState', data.state);
                console.log(event.data);

                //what is done with the data depends on the state
                //STATE_READ state
                if(data.state == 1)
                {
                  if(_this.$store.getters.getInputDisabled){
                    _this.$store.dispatch('disableInput', false);
                  }

                  if(_this.$store.getters.getHardLimit){
                    _this.$store.dispatch('setHardLimit', false);
                  }

                  let gauge_data = {"load_cell":data.load_cell,"gauge_1":data.gauge_1,"gauge_2":data.gauge_2,"gauge_3":data.gauge_3,"gauge_4":data.gauge_4,"gauge_5":data.gauge_5,"gauge_6":data.gauge_6}
                  this.highlighted_index = this.getUpdatedIndex(gauge_data, this.getCurrent);
                  this.$store.dispatch('setCurrent', gauge_data);
                } 
                else if(data.state == 3 || data.state == 4 || data.state == 5 || data.state == 6 || data.state == 7){
                  //hardware is resetting, so disable input
                  _this.$store.dispatch('disableInput', true);
                }
                else if(data.state == 10){
                    // hardware limit switch has been hit, so display a message that user needs to reset hardware before continuing.
                    _this.$store.dispatch('setHardLimit', true);
                }
                else{
                  this.highlighted_index = [];  //don't highlight any of the gauge values
                  console.log('no change');
                }
                
            } catch (e) {
                if(debug){
                    console.log(e)
                }
                
            }
        }

		
		
		},
  }
}
</script>

<style scoped>

img{
    max-width: 100%;
    max-height: auto;
}

#overlay{
  position:relative;
}

.highlighted{
  background-color:rgba(251, 255, 0, 0.952);
}

.unhighlighted{
  background-color:rgba(255, 255, 255, 0.5);
}

#load-cell{
  position:absolute;
  top:80%;
  left:85%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  
}

#gauge-1{
  position:absolute;
  top:40%;
  left:80%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  
}

#gauge-2{
  position:absolute;
  top:65%;
  left:70%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  
}

#gauge-3{
  position:absolute;
  top:35%;
  left:50%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  
}

#gauge-4{
  position:absolute;
  top:5%;
  left:30%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  
}

#gauge-5{
  position:absolute;
  top:35%;
  left:30%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  
}

#gauge-6{
  position:absolute;
  top:65%;
  left:30%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  
}


</style>
