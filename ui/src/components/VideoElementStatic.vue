//Vue3 updated

//HAS TEMPORARY CODE AT THE MOMENT FOR SIMPLY GETTING THE VIDEO CAMERA POSITION REGARDING THE TRUSS

<template>
<!-- temp -->
<canvas id="video-canvas"></canvas>

<div v-if='getSessionExpired' class='row'>
  <div class='col-12'>
        <img id='session-end-image' src='https://assets.practable.io/images/common/thank-you-screen.svg' alt='session ended'>
    </div>
</div>
<div v-else>
  <div class='row' >
      <div class='col-12' id='video-element'>
        <!-- <canvas id="video-canvas"></canvas> --> 
        <toolbar parentCanvasID="video-canvas" parentComponentName="webcam" parentDivID='video-element' :showDownload='true' :showPopupHelp='false' :showOptions="false"></toolbar>
      </div>
    </div>
</div>    
    
    
    <!-- <div id='videoWrapper'></div> -->
</template>

<script>
import JSMpeg from "@cycjimmy/jsmpeg-player";
import { mapGetters } from 'vuex';
import Toolbar from "./elements/Toolbar.vue";

export default {
  name: "VideoElementStatic",
  props: ["url"],
  components:{
      Toolbar,
    },
  data(){
      return{
        player: null,
      }
    },
  computed:{
    ...mapGetters([
      'getSessionExpired',
    ]),
  },
  mounted() {
    // TEMP in mounted
    let canvas = document.getElementById("video-canvas");
        //temporary static URL for video access
        let url = "wss://video.practable.io:443/in/dpr/video0";
        this.player = new JSMpeg.Player(url, {canvas: canvas, preserveDrawingBuffer: true});
        //this.player = new JSMpeg.Player(this.url, {canvas: canvas, preserveDrawingBuffer: true});
  },
  watch:{
    url(){
        let canvas = document.getElementById("video-canvas");
        //temporary static URL for video access
        let url = "wss://video.practable.io:443/in/dpr/video0";
        this.player = new JSMpeg.Player(url, {canvas: canvas, preserveDrawingBuffer: true});
        //this.player = new JSMpeg.Player(this.url, {canvas: canvas, preserveDrawingBuffer: true});

    },
  },
  methods:{
    
  }
    
};


</script>

<style>
    #video-canvas {
      width: 100%;
      /* height: 100%; */
    }

    #session-end-image {
      width: 100%;
      /* height: 100%; */
    }

</style>