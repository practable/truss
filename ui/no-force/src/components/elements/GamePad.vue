
<template>
  <div class='row' id='control-dial-div'>
      <img id='dial-image' src='/images/control-dial-real.png' hidden>
      <img id='dial-background-image' src='/images/control-dial-background-real.png' hidden>

      <input class='form-control text-center' type='text' id='dial-output' :value='motor_position.toFixed(2)' readonly>
      <canvas class='col-12' id='control-dial-canvas'  ></canvas>
      <audio id='click-sound'>
        <source src='/audio/click.wav' type="audio/mpeg" preload='auto' />
          <!-- <source src='../../../public/audio/click2.mp3' type="audio/mpeg" preload='auto' /> -->
      </audio>
  </div>

    <!-- <toolbar parentCanvasID="" parentComponentName="control-dial" parentDivID="control-dial-div" :showDownload='true' :showPopupHelp="true" :showOptions="false">  
        
        <template v-slot:popup id='control-dial-popup'>
            <div class='row mb-2' id='controller-layout'>
                <div class='col-4'>
                    <img class='popup-image' src='../../../public/images/controllerLayout.png'>
                </div>
                <div class='col-8'>
                    <h3> Controller Layout</h3>
                    <p> When using a gamepad, the (left) analog stick is used to set the position of the motor. A/Square is used to send the position to the hardware.
                        B/Circle is used to reset the hardware. Y/Triangle is used to Tare the gauges.
                    </p>
                </div>
            </div>

            
        </template>
    </toolbar> -->
</template>

<script>
import { mapActions } from 'vuex';
//import Toolbar from './Toolbar.vue';

var canvas;
var ctx;
let dial = new Image();
let background = new Image();
let sound;

export default {

  name: 'GamePad',
  props:{
      
  },
  components: {
        //Toolbar,
    },
  emits:['onchange'],
  data () {
    return {
        rotating: false,
        angle: 0.00,    //rads
        motor_position: 0.00,
        angleToPosFactor: 2000,
        image_ratio: 1.3333,   //0.8 for original images, 1.3333 for new "real" images
        gamepad_previous_vector: {x:0, y:0},
        gamepad_current_vector: {x:0,y:0},
        min_angle: 0,
        max_angle: 10.0,  //rads
        min_angular_factor: 0.05, 
        difficulty_factor: 1.0,
        previous_played_angle: 0.00,
        play_sound_angular_frac: 0.1,
        count_plays: 0,
        previous_touch: null,

        gamepad_connected: false,
        x_axis: 0.0,
        y_axis: 0.0,
        current_button: null,
        previous_button: null,
        interval_id: '',
        movementX: 0.0,
        movementY: 0.0,
    }
  },
  created(){
      
  },
  mounted(){
      let _this = this;
      window.addEventListener("gamepadconnected", function(e) {
          console.log("Gamepad connected at index %d: %s. %d buttons, %d axes.",
          e.gamepad.index, e.gamepad.id, e.gamepad.buttons.length, e.gamepad.axes.length);
          _this.gamepad_connected = true;
        });

        window.addEventListener("gamepaddisconnected", function(e) {
            console.log(e);
            _this.gamepad_connected = false;
        });

      canvas = document.getElementById("control-dial-canvas");
      ctx = canvas.getContext("2d");
      canvas.height = canvas.width * this.image_ratio;

      dial.onload = () => {
        let x = 0;
        let y= 0;
        let w=canvas.width;
        let h=canvas.height;
        ctx.drawImage(dial, x, y, w, h);
        this.draw(); //gets the image order
      }
      dial.src = document.getElementById("dial-image").src;

      background.onload = () => {
        let x = 0;
        let y= 0;
        let w=canvas.width;
        let h=canvas.height;
        ctx.drawImage(background, x, y, w, h);
        this.draw(); //gets the image order
      }
      background.src = document.getElementById("dial-background-image").src;
      sound = document.getElementById('click-sound');
      //sound.load();

      
  },
  computed:{
      
  },
  watch:{
      gamepad_connected(connected){
          if(connected){
              this.interval_id = setInterval(() => {
                this.getButtons();
                this.getAxes();
            }, 100);
          } else{
              clearInterval(this.interval_id);
          }
      }
  },
  
  methods: {
    ...mapActions([
          'setDraggable'
      ]),
      draw() {
        canvas.height = canvas.width * this.image_ratio;

        ctx.clearRect(0,0,canvas.width, canvas.height);
        ctx.save();
        ctx.drawImage(background, 0, 0, canvas.width, canvas.height);
        //ctx.translate(canvas.x, canvas.y);
        ctx.translate(canvas.width/2, canvas.height/2)
        ctx.rotate(this.angle);
        ctx.translate(-canvas.width/2, -canvas.height/2)
        ctx.drawImage(dial, 0,0, canvas.width, canvas.height);
        ctx.restore();
        
        },
      rotateDial(){  
        if(Math.abs(this.x_axis) > 0.9 || Math.abs(this.y_axis) > 0.9){
          
          if(this.previous_played_angle/this.angle < 0){
            //sound.play();
            this.previous_played_angle = this.angle;
          } else if(Math.abs(this.angle) > Math.abs(this.previous_played_angle) && Math.abs(this.angle) > Math.abs(this.previous_played_angle) + this.play_sound_angular_frac*(this.max_angle - Math.abs(this.previous_played_angle))){
              sound.play();
              this.count_plays++;
              this.previous_played_angle = this.angle;
          } else if(Math.abs(this.angle) < Math.abs(this.previous_played_angle) && Math.abs(this.angle) < Math.abs(this.max_angle) - Math.abs(this.max_angle)*Math.pow((1-this.play_sound_angular_frac), this.count_plays)){ 
              sound.play();
              this.count_plays--;
              this.previous_played_angle = this.angle;
          }
          
          this.gamepad_current_vector = this.getAxesCurrentVector();
          let axes_x = this.gamepad_current_vector.x;
          let axes_y = this.gamepad_current_vector.y;
          let angle = this.angleBetweenVectors(this.gamepad_current_vector, this.gamepad_previous_vector);

          let distance_to_click = Math.sqrt(axes_x*axes_x + axes_y*axes_y);
          let angular_distance_to_max = this.max_angle - Math.abs(this.angle);
          let angular_factor = (angular_distance_to_max > this.min_angular_factor) ? angular_distance_to_max : this.min_angular_factor;
          let angle_to_move = angle*distance_to_click*angular_factor/(this.difficulty_factor*canvas.clientWidth/2);   //movement angle is decreased the further towards the centre you move the mouse.

            if(this.gamepad_current_vector.x > this.gamepad_previous_vector.x){
                this.movementX = 1;
            } else if(this.gamepad_current_vector.x < this.gamepad_previous_vector.x){
                this.movementX = -1;
            } else {
                this.movementX = 0;
            }

            if(this.gamepad_current_vector.y > this.gamepad_previous_vector.y){
                this.movementY = 1;
            } else if(this.gamepad_current_vector.y < this.gamepad_previous_vector.y){
                this.movementY = -1;
            } else {
                this.movementY = 0;
            }

          if(axes_x > 0 && axes_y < 0){
            if(this.movementY > 0 || this.movementX > 0){
              if(this.angle + angle_to_move < this.max_angle){
                this.angle += angle_to_move;
              }
            } else if(this.movementY < 0 && this.movementX < 0){
                if(this.angle - angle_to_move > this.min_angle){
                  this.angle -= angle_to_move;
                }
            }
          } 
          else if(axes_x > 0 && axes_y > 0){
            if(this.movementY > 0 || this.movementX < 0){
              if(this.angle + angle_to_move < this.max_angle){
                this.angle += angle_to_move;
              }
            } else if(this.movementY < 0 || this.movementX > 0){
                if(this.angle - angle_to_move > this.min_angle){
                  this.angle -= angle_to_move;
                }
            }
          } 
          else if(axes_x < 0 && axes_y > 0){
            if(this.movementY < 0 || this.movementX < 0){
              if(this.angle + angle_to_move < this.max_angle){
                this.angle += angle_to_move;
              }
            } else if(this.movementY > 0 || this.movementX > 0){
                if(this.angle - angle_to_move > this.min_angle){
                  this.angle -= angle_to_move;
                }
            }
          } 
          else if(axes_x < 0 && axes_y < 0){
            if(this.movementY < 0 || this.movementX > 0){
                if(this.angle + angle_to_move < this.max_angle){
                  this.angle += angle_to_move;
                }
            } else if(this.movementY > 0 || this.movementX < 0){
                if(this.angle - angle_to_move > this.min_angle){
                  this.angle -= angle_to_move;
                }
            }
          }
          
          
          this.gamepad_previous_vector = this.gamepad_current_vector;
          this.motor_position = this.angle * this.angleToPosFactor;
          this.draw();        //redraw the shapes only when attempting to move
        }
      },
      getAxesCurrentVector(){
        let vector = {x:this.x_axis,y:this.y_axis};
        
        return vector;
        
      },
      angleBetweenVectors(a, b){
        let scalar_prod = a.x*b.x + a.y*b.y;
        let mag_a = Math.sqrt(a.x*a.x + a.y*a.y);
        let mag_b = Math.sqrt(b.x*b.x + b.y*b.y);

        return Math.acos(scalar_prod/(mag_a*mag_b));
      },
        getButtons(){
          let gp;
           if(navigator.webkitGetGamepads) {
               gp = navigator.webkitGetGamepads()[0];
               if(gp.buttons[0] == 1) {
                   this.current_button = 'a';
                } else if(gp.buttons[1] == 1){
                    this.current_button = 'b';
                } else if(gp.buttons[3] == 1){
                    this.current_button = 'y';
                }else {
                    this.current_button = null;
                }
        } else {
            gp = navigator.getGamepads()[0];

            if(gp.buttons[0].value > 0 || gp.buttons[0].pressed == true) {
                this.current_button = 'a';
            } else if(gp.buttons[1].value > 0 || gp.buttons[1].pressed == true) {
                this.current_button = 'b';
            } else if(gp.buttons[3].value > 0 || gp.buttons[3].pressed == true) {
                this.current_button = 'y';
            }else {
                this.current_button = null;
            }
        }

            if(this.previous_button == null && this.current_button == 'a')
            {
                this.$emit('onchange', this.motor_position);
            } 
            else if(this.previous_button == null && this.current_button == 'b')
            {
                this.$emit('reset');
            }
            else if(this.previous_button == null && this.current_button == 'y')
            {
                this.$emit('tare');
            }

            this.previous_button = this.current_button;

      },
      getAxes(){
          
          let gp;
           if(navigator.webkitGetGamepads) {
               gp = navigator.webkitGetGamepads()[0];
            
               this.x_axis = gp.axes[0];
               this.y_axis = gp.axes[1];
            } else {
                gp = navigator.getGamepads()[0];
                
                this.x_axis = gp.axes[0];
                this.y_axis = gp.axes[1];
            }

            this.rotateDial();
      },
  }
}
</script>

<style scoped>


#control-dial-canvas{
  touch-action: none;
}



</style>