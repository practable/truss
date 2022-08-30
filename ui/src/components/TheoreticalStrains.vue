<template>
        
    <div class='container-fluid m-2 background-white border rounded'>
      <div class='col-sm-12' id='overlay-theory'>
        
            <img id='truss-theory-image' src='../../public/images/theoretical-truss.png' alt='truss-theory-image'>

            <div id='load-theory-cell'>{{getCurrent.load_cell}} N</div>

            <div id='gauge-theory-1'>{{gaugeTheory_1.toFixed(1)}} &mu;&epsilon;</div>

            <div id='gauge-theory-2'>{{gaugeTheory_2.toFixed(1)}} &mu;&epsilon;</div>

            <div id='gauge-theory-3'>{{gaugeTheory_3.toFixed(1)}} &mu;&epsilon;</div>

            <div id='gauge-theory-4'>{{gaugeTheory_4.toFixed(1)}} &mu;&epsilon;</div>

            <div id='gauge-theory-5'>{{gaugeTheory_5.toFixed(1)}} &mu;&epsilon;</div>

            <div id='gauge-theory-6'>{{gaugeTheory_6.toFixed(1)}} &mu;&epsilon;</div>

      </div>

      <toolbar parentCanvasID="" parentComponentName="display-theory" parentDivID="overlay-theory" :showDownload='true' :showPopupHelp="true" :showOptions="false">  
        
        <template v-slot:popup id='display-theory-popup'>
            <div class='row mb-2' id='help-div'>
          
              <h3> Theoretical values display </h3>
              <p> A display of the theoretical truss and values of strain calculated for the measured load force.</p>
                
            </div>
        </template>

    </toolbar>
     
    </div>   

</template>

<script>
import { mapGetters } from 'vuex';
import Toolbar from './elements/Toolbar.vue';

export default {

  name: 'TheoreticalStrains',
  components: {
    Toolbar,
  },
  data(){
    return {
        load_position: 0,       //for different positions of the loading cell: 0 => L2
        beam_width: 0.025,
        beam_breadth: 0.010,
        beam_modulus: 2.5E9,
    }
  },
  mounted(){
    
  },
  computed:{
    ...mapGetters([
			'getCurrent',
		]),
        getLoad(){
            return this.getCurrent.load_cell;
        },
        cross_section(){
            return this.beam_width*this.beam_breadth;
        },
        gaugeTheory_1(){
            let strain;
            if(this.load_position == 0){
                let force = this.getLoad/Math.sin(Math.PI/4);
                strain = this.strainFromForce(force);
            }

            return strain;
        },
        gaugeTheory_2(){
            let strain;
            if(this.load_position == 0){
                let force = -this.getLoad;
                strain = this.strainFromForce(force);
            }

            return strain;
        },
        gaugeTheory_3(){
            let strain;
            if(this.load_position == 0){
                let force = -this.getLoad;
                strain = this.strainFromForce(force);
            }

            return strain;
        },
        gaugeTheory_4(){
            let strain;
            if(this.load_position == 0){
                let force = this.getLoad;
                strain = this.strainFromForce(force);
            }

            return strain;
        },
        gaugeTheory_5(){
            let strain;
            if(this.load_position == 0){
                let force = this.getLoad/Math.sin(Math.PI/4);
                strain = this.strainFromForce(force);
            }

            return strain;
        },
        gaugeTheory_6(){
            let strain;
            if(this.load_position == 0){
                let force = -2*this.getLoad;
                strain = this.strainFromForce(force);
            }

            return strain;
        }
  },
  methods:{
      //returns strain in micro-strain units
    strainFromForce(force){
        return force*1000000.0/(this.beam_modulus*this.cross_section);
    }
  }
}
</script>

<style>
img{
    max-width: 100%;
    max-height: auto;
}

#overlay-theory{
  position:relative;
}

#load-theory-cell{
  position:absolute;
  top:90%;
  left:80%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  background-color:rgba(255, 255, 255, 0.5);
}

#gauge-theory-1{
  position:absolute;
  top:45%;
  left:75%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  background-color:rgba(255, 255, 255, 0.5);
}

#gauge-theory-2{
  position:absolute;
  top:70%;
  left:67.5%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  background-color:rgba(255, 255, 255, 0.5);
}

#gauge-theory-3{
  position:absolute;
  top:50%;
  left:55%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  background-color:rgba(255, 255, 255, 0.5);
}

#gauge-theory-4{
  position:absolute;
  top:15%;
  left:30%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  background-color:rgba(255, 255, 255, 0.5);
}

#gauge-theory-5{
  position:absolute;
  top:45%;
  left:35%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  background-color:rgba(255, 255, 255, 0.5);
}

#gauge-theory-6{
  position:absolute;
  top:70%;
  left:27.5%;
  width: 10%;
  height:5%;
  min-width: 50px;
  min-height: 50px;
  z-index: 2;
  background-color:rgba(255, 255, 255, 0.5);
}
</style>