<template>
<div class='container-fluid m-2 background-white border rounded'>
    <div id='load-control'>
        
        <div class='row'>
            <div class='col-sm-6 mb-2'>
                <img id='load-cell-image' src='../../public/images/load-cell.jpg' alt='load-cell-image'>
            </div>
        </div>

        <div class="input-group" @mousedown='setDraggable(false)' @mouseup="setDraggable(true)">
            <span class="input-group-text">Enter command</span>
            <textarea class="form-control" aria-label="Enter command" v-model='command_text'></textarea>
            <button class="btn btn-primary" type="button" @click='sendCommand'>Send</button>
            <button class="btn btn-danger" type="button" @click='command_text = ""; error_text = ""'>Clear</button>
            
        </div>

        <div v-if='error_text != ""'><p>{{error_text}}</p> </div>


    </div>

    <toolbar parentCanvasID="" parentComponentName="load-control" parentDivID="load-control" :showDownload='false' :showPopupHelp="true" :showOptions="false">  
        
        <template v-slot:popup>
            <div class='row mb-2' id='help-div'>
          
              <h3> Commands </h3>
              <p> {set: position, to: <i>value</i>} => <i>value</i> is a positive number between 0 and 200</p>
                <p>  {set: mode, to: read} </p>
                  <p>{set: mode, to: move} </p>
                  <p>{set: mode, to:standby} </p>
                  <p>{set: mode, to:zero}</p>
                  <p>{set: mode, to:tare} </p>
                
            </div>
        </template>

    </toolbar>
  </div>
</template>

<script>
import { mapGetters, mapActions } from 'vuex';
import Toolbar from './elements/Toolbar.vue';

export default {
    name: 'LoadControlCode',
    components: {
        Toolbar,
    },
    props:[],
    data(){
        return {
            servo_input: 0.00,
            command_text: '',
            error_text: ''
        }
    },
    mounted(){
        this.servo_input = this.getServoPosition;
    },
    computed:{
        ...mapGetters([
                'getServoPosition',
                'getDataSocket'
            ]),
            
    },
    methods:{
        ...mapActions([
            'setDraggable',
            
        ]),
        sendCommand(){
            this.error_text = '';

            if(this.checkValidCommand(this.command_text)){
                //console.log(this.command_text);
                this.getDataSocket.send(this.command_text);
            } else{
                console.log('Invalid command');
            }
            
        },
        checkValidCommand(command){
            //check for curly brackets
            if(command.indexOf('{') == 0 && command.indexOf('}') == command.length - 1)
            {
                if(command.includes("set"))
                {
                    if(command.includes("to"))
                    {
                        return true;
                    } else{
                        console.log('Missing the "to" parameter');
                        this.error_text = 'Missing the "to" parameter';
                        return false;
                    }
                } else{
                    console.log('Missing the "set" parameter');
                    this.error_text = 'Missing the "set" parameter';
                    return false;
                }
            } else {
                console.log('Missing { or } at start and end of command');
                this.error_text = 'Missing { or } at start and end of command';
                return false;
            }
        }
    }
}
</script>

<style>

</style>