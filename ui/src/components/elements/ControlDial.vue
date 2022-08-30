
<template>
  <div class='row' id='control-dial-div'>
      <img id='dial-image' src='../../../public/images/control-dial-real.png' hidden>
      <img id='dial-background-image' src='../../../public/images/control-dial-background-real.png' hidden>

      <input class='form-control text-center' type='text' id='dial-output' :value='motor_position.toFixed(2)' readonly>
      <canvas class='col-12' id='control-dial-canvas' @mousedown='(event) => {toggleClicked(event); setDraggable(false)}' @mouseleave='(event) => {if(clicked){toggleClicked(event); setDraggable(true);}}' @mouseup="(event) => {toggleClicked(event); setDraggable(true); sendValue();}" @mousemove="rotateDial" 
              @touchstart='(event) => {toggleClicked(event); setDraggable(false)}' @touchend="(event) => {toggleClicked(event); setDraggable(true); sendValue();}" @touchcancel='(event) => {if(clicked){toggleClicked(event); setDraggable(true);}}' @touchmove="rotateDial"></canvas>
      <audio id='click-sound'>
        <source src='../../../public/audio/click.wav' type="audio/mpeg" preload='auto' />
          <!-- <source src='../../../public/audio/click2.mp3' type="audio/mpeg" preload='auto' /> -->
      </audio>
  </div>
</template>

<script>
import { mapActions } from 'vuex';

var canvas;
var ctx;
let dial = new Image();
let background = new Image();
let sound;

export default {

  name: 'ControlDial',
  props:{
      
  },
  emits:['onchange'],
  data () {
    return {
        clicked: false,
        angle: 0.00,    //rads
        motor_position: 0.00,
        angleToPosFactor: 2000,
        image_ratio: 1.3333,   //0.8 for original images, 1.3333 for new "real" images
        mouse_click_previous_vector: {x:0, y:0},
        mouse_click_current_vector: {x:0,y:0},
        min_angle: 0,
        max_angle: 10.0,  //rads
        min_angular_factor: 0.05, 
        difficulty_factor: 6.0,
        previous_played_angle: 0.00,
        play_sound_angular_frac: 0.5,
        count_plays: 0,
        previous_touch: null,
    }
  },
  components: {
    
  },
  created(){
      
  },
  mounted(){
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
      rotateDial(event){  
        if(this.clicked){
          
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
          
          this.mouse_click_current_vector = this.getMouseCurrentVector(event);
          let mouse_x = this.mouse_click_current_vector.x;
          let mouse_y = this.mouse_click_current_vector.y;
          let angle = this.angleBetweenVectors(this.mouse_click_current_vector, this.mouse_click_previous_vector);

          let distance_to_click = Math.sqrt(mouse_x*mouse_x + mouse_y*mouse_y);
          let angular_distance_to_max = this.max_angle - Math.abs(this.angle);
          let angular_factor = (angular_distance_to_max > this.min_angular_factor) ? angular_distance_to_max : this.min_angular_factor;
          let angle_to_move = angle*distance_to_click*angular_factor/(this.difficulty_factor*canvas.clientWidth/2);   //movement angle is decreased the further towards the centre you move the mouse.

          //for mobile=====
          if(event.type == 'touchmove'){
            let touch = event.touches[0];

            if(this.previous_touch){
              event.movementX = touch.pageX - this.previous_touch.pageX;
              event.movementY = touch.pageY - this.previous_touch.pageY;
            }

            this.previous_touch = touch;
            
          }
          //=====

          if(mouse_x > 0 && mouse_y < 0){
            if(event.movementY > 0 || event.movementX > 0){
              if(this.angle + angle_to_move < this.max_angle){
                this.angle += angle_to_move;
              }
            } else if(event.movementY < 0 && event.movementX < 0){
                if(this.angle - angle_to_move > this.min_angle){
                  this.angle -= angle_to_move;
                }
            }
          } 
          else if(mouse_x > 0 && mouse_y > 0){
            if(event.movementY > 0 || event.movementX < 0){
              if(this.angle + angle_to_move < this.max_angle){
                this.angle += angle_to_move;
              }
            } else if(event.movementY < 0 || event.movementX > 0){
                if(this.angle - angle_to_move > this.min_angle){
                  this.angle -= angle_to_move;
                }
            }
          } 
          else if(mouse_x < 0 && mouse_y > 0){
            if(event.movementY < 0 || event.movementX < 0){
              if(this.angle + angle_to_move < this.max_angle){
                this.angle += angle_to_move;
              }
            } else if(event.movementY > 0 || event.movementX > 0){
                if(this.angle - angle_to_move > this.min_angle){
                  this.angle -= angle_to_move;
                }
            }
          } 
          else if(mouse_x < 0 && mouse_y < 0){
            if(event.movementY < 0 || event.movementX > 0){
                if(this.angle + angle_to_move < this.max_angle){
                  this.angle += angle_to_move;
                }
            } else if(event.movementY > 0 || event.movementX < 0){
                if(this.angle - angle_to_move > this.min_angle){
                  this.angle -= angle_to_move;
                }
            }
          }
          
          
          this.mouse_click_previous_vector = this.mouse_click_current_vector;
          this.motor_position = this.angle * this.angleToPosFactor;
          this.draw();        //redraw the shapes only when attempting to move
        }
      },
      toggleClicked(event){
        this.clicked = !this.clicked;
        
        //for mobile=====
        if(event.type == 'touchstart'){
          event = event.touches[0];
        }
        //=======
        let image_centre_x = canvas.clientWidth/2;
        let image_centre_y = canvas.clientHeight/2;
        let mouse_x = event.pageX - canvas.offsetLeft;
        let mouse_y = event.pageY - canvas.offsetTop;

        this.mouse_click_previous_vector.x = mouse_x - image_centre_x;
        this.mouse_click_previous_vector.y = mouse_y - image_centre_y;
      },
      getMouseCurrentVector(event){
        //for mobile======
        if(event.type == 'touchmove'){
          event = event.touches[0];
        }
        //========
        let image_centre_x = canvas.clientWidth/2;
        let image_centre_y = canvas.clientHeight/2;
        let mouse_x = event.pageX - canvas.offsetLeft;
        let mouse_y = event.pageY - canvas.offsetTop;

        let vector = {x:0,y:0};
        vector.x = mouse_x - image_centre_x;
        vector.y = mouse_y - image_centre_y;
        
        return vector;
        
      },
      angleBetweenVectors(a, b){
        let scalar_prod = a.x*b.x + a.y*b.y;
        let mag_a = Math.sqrt(a.x*a.x + a.y*a.y);
        let mag_b = Math.sqrt(b.x*b.x + b.y*b.y);

        return Math.acos(scalar_prod/(mag_a*mag_b));
      },
      sendValue(){
        this.$emit('onchange', this.motor_position);
      },

  }
}
</script>

<style scoped>
/* #control-dial-div {
	position: relative;
	width: 100%;		
  padding-top: 100%;
} */

/* #control-dial{
  position: absolute;
  top: 0;
  left: 0;
  bottom: 0;
  right: 0;
} */

#control-dial-canvas{
  touch-action: none;
}



</style>