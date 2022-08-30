//Controls stepper motor. Stepper between 0 and 20000 steps.

<template>
    <div class='container-fluid m-2 background-white border rounded' id='load-control'>
        <div class='d-grid gap-2 d-sm-block mb-2'>
            <!-- <button type='button' class='btn btn-sm btn-primary me-2' @click='read'>Start Reading</button>
            <button type='button' class='btn btn-sm btn-danger me-2' @click='standby'>Standby</button> -->
            <button type='button' class='button-sm button-danger me-2' @click='reset' :disabled='getInputDisabled'>Reset</button>
            <button type='button' class='button-sm button-secondary me-2' @click='tare' :disabled='getInputDisabled'>Tare</button>
            <button type='button' class='button-sm button-secondary me-2' @click='tareLoad' :disabled='getInputDisabled' v-if='getShowForce'>Tare Load</button>

        </div>

        <div v-if='getInputDisabled' class='row justify-content-center txt-lg txt-primary' >
            {{ this.message }}
        </div>
        <div v-if='getHardLimit' class='row justify-content-center txt-lg txt-primary' >
            {{ this.limit_message }}
        </div>


        <div class='row'>
            <div class='col-sm-6 mb-2'>
                <!-- <img id='load-cell-image' src='../../public/images/load-cell.jpg' alt='load-cell-image'> -->
                <webcam-stream id='webcam-stream' />
            </div>
            
            <!-- Simple input for load control -->
            <div v-if='controlVersion == 0' class='col-sm-6 btn-group-vertical'>
                <div class="input-group mb-3">
                    <input type="text" id='load-input' class='input' aria-describedby="button-addon2" v-model='servo_input' :min='servo_min' :max='servo_max' @keyup.enter='setPositionAndMove(servo_input)' @mousedown='setDraggable(false)' @mouseup='setDraggable(true)'>
                    <button :class='getButtonClass' type="button" id="set-pos-button" @submit.prevent @click='setPositionAndMove(servo_input)' :disabled='getInputDisabled'>Set</button>
                </div>
            </div>

            <!-- Slider for load control -->
             <div v-else-if='controlVersion == 1' class='col-sm-6 btn-group-vertical'>
                 <div class="input-group mb-3 justify-content-center">
                    <input class='form-range m-2' type="range" id='load-input' v-model='servo_input' :min='servo_min' :max='servo_max' step='1' @change='setPositionAndMove(servo_input)' @mousedown='setDraggable(false)' @mouseup='setDraggable(true)' :disabled='getInputDisabled'>
                    <input class='input' type='text' id='load-output' :value='servo_input' readonly>
                
                    <div class='row m-2'>
                        <div class='col-4 mb-2'><button type='button' class='button-toolbar button-primary' @click='incrementMove(-10)' :disabled='getInputDisabled'>-</button></div>
                        <div class='col-4 mb-2'><label type='label' class='col-form-label'>10</label></div>
                        <div class='col-4 mb-2'><button type='button' class='button-toolbar button-primary' @click='incrementMove(10)' :disabled='getInputDisabled || servo_input >= servo_max'>+</button></div>

                        <div class='col-4 mb-2'><button type='button' class='button-toolbar button-primary' @click='incrementMove(-5)' :disabled='getInputDisabled'>-</button></div>
                        <div class='col-4 mb-2'><label class='col-form-label'>5</label></div>
                        <div class='col-4 mb-2'><button type='button' class='button-toolbar button-primary' @click='incrementMove(5)' :disabled='getInputDisabled || servo_input >= servo_max'>+</button></div>

                        <div class='col-4 mb-2'><button type='button' class='button-toolbar button-primary' @click='incrementMove(-1)' :disabled='getInputDisabled'>-</button></div>
                        <div class='col-4 mb-2'><label class='col-form-label'>1</label></div>
                        <div class='col-4 mb-2'><button type='button' class='button-toolbar button-primary' @click='incrementMove(1)' :disabled='getInputDisabled || servo_input >= servo_max'>+</button></div>
                    </div>
                   
                
                
                </div>
                
            </div>

            <!-- Skeumorphic display - "real" control dial -->
            <div v-else-if='controlVersion == 2' class='col-sm-6' :disabled='getInputDisabled'>
                <control-dial @onchange='(value) => setPositionAndMove(value)' :disabled='getInputDisabled'/>
            </div>

            <!-- Physical controller -->
            <div v-else-if='controlVersion == 4' class='col-sm-6' :disabled='getInputDisabled'>
                <game-pad @onchange='(value) => setPositionAndMove(value)' @reset='reset' @tare='tare' :disabled='getInputDisabled'/>
            </div>

        </div>

        <toolbar parentCanvasID="" parentComponentName="control" parentDivID="load-control" :showDownload='false' :showPopupHelp="true" :showOptions="false">  
        
        <template v-slot:popup id='control-popup'>
            <div class='row mb-2' id='control-div'>
                <div class='col-12'>
                    <h3> Load control </h3>
                    <p> The load force is controlled using a linear actuator. The actuator can be positioned between 0 and 20. Reset will return the stepper to 
                        its 0 position. Tare will zero the gauge measurements.
                    </p>
                </div>

                <div v-if='controlVersion == 0' class='col-12'>
                    <h3> Control Mode </h3>
                    <p> A simple input field allows you to position the stepper between 0 and 20 000 steps by typing in the desired position.
                    </p>
                </div>

                <div v-else-if='controlVersion == 1' class='col-12'>
                    <h3> Control Mode </h3>
                    <p> A simple slider allows you to position the stepper between 0 and 20. Smaller changes can be chosen using the 
                        buttons below the slider.
                    </p>
                </div>

                <div v-else-if='controlVersion == 2' class='col-12'>
                    <h3> Control Mode </h3>
                    <p> A "realistic" control dial is used to set the stepper motor position. Click and drag the mouse or touch and drag with a finger on mobile devices to rotate the 
                        control dial. Releasing the mouse (or lifting your finger) will then set the motor to the desired position. The control dial gets harder to rotate as it gets closer 
                        to the maximum value.
                    </p>
                </div>

                 <div v-else-if='controlVersion == 4' class='row'>
                    <div class='col-4'>
                        <img class='popup-image' src='../../public/images/controllerLayout.png'>
                    </div>
                    <div class='col-8'>
                        <h3> Controller Layout</h3>
                        <p> When using a gamepad, the (left) analog stick is used to set the position of the motor. A/Square is used to send the position to the hardware.
                            B/Circle is used to reset the hardware. Y/Triangle is used to Tare the gauges.
                        </p>
                    </div>
                </div>


            </div>


            </template>
        </toolbar>


    </div>
  
</template>

<script>
import { mapGetters, mapActions } from 'vuex';
import ControlDial from './elements/ControlDial.vue';
import WebcamStream from './WebcamStream.vue';
import GamePad from './elements/GamePad.vue';
import Toolbar from './elements/Toolbar.vue';

export default {
    name: 'LoadControl',
    components: {
        ControlDial,
        WebcamStream,
        GamePad,
        Toolbar,
    },
    props:['controlVersion'],
    data(){
        return {
            servo_input: 0.00,
            servo_max: 15,
            servo_min: 0,

            message: 'Please wait, resetting hardware...',
            limit_message: 'The hardware limit has been reached, returning to safe position. Please reset to continue.'
        }
    },
    mounted(){
        
    },
    computed:{
        ...mapGetters([
                'getServoPosition',
                'getInitialPosition',
                'getInputDisabled',
                'getShowForce',
                'getHardLimit'
            ]),
        getButtonClass(){
            if(this.getServoPosition == this.servo_input){
                return 'btn btn-secondary'
            } else {
                return 'btn btn-success'
            }
        }
            
    },
    watch:{
        getInitialPosition(value){
            console.log('servo position=');
            console.log(value);
            this.servo_input = value;
        }
    },
    methods:{
        ...mapActions([
            'setDraggable',
            'standby',
            'read',
        ]),
        setMessage(mes){
            this.message = mes;
        },
        setPositionAndMove(input){
            console.log('sending to servo = ' + input);
            this.$store.dispatch('setPositionAndMove', input);
        },
        incrementMove(delta){
            let new_servo_pos = Number(this.servo_input) + Number(delta);
            if(new_servo_pos <= this.servo_max && new_servo_pos >= this.servo_min)
            {
                this.servo_input = new_servo_pos;
                this.setPositionAndMove(this.servo_input);
            }
          
        },
        tare(){
            this.setMessage('Please wait, gauges taring...');
            let tare_data = {load_cell: this.$store.getters.getCurrent.load_cell, gauge_1: 0.0, gauge_2: 0.0, gauge_3: 0.0, gauge_4: 0.0, gauge_5: 0.0, gauge_6: 0.0};
            this.$store.dispatch('setCurrent', tare_data);
            this.$store.dispatch('tare');
        },
        reset(){
            this.setMessage('Please wait, resetting hardware...');
            this.$store.dispatch('reset');
            this.servo_input = 0.00;
        },
        tareLoad(){
            this.setMessage('Please wait, load cell taring...');
            let tare_data = {load_cell: 0.0, gauge_1: this.$store.getters.getCurrent.gauge_1, gauge_2: this.$store.getters.getCurrent.gauge_2, gauge_3: this.$store.getters.getCurrent.gauge_3, gauge_4: this.$store.getters.getCurrent.gauge_4, gauge_5: this.$store.getters.getCurrent.gauge_5, gauge_6: this.$store.getters.getCurrent.gauge_6};
            this.$store.dispatch('tareLoad');
            this.$store.dispatch('setCurrent', tare_data);
        }
    }
}
</script>

<style>

</style>