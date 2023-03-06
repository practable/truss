//Updated to Vue3, removing eventBus implementation, instead nav bar emits up to App which then controls components and passes props to sibling components

<template>

  <nav class="navbar fixed-top navbar-expand-lg navbar-dark background-primary">
    <div class="container-fluid">
        <img src="/images/practable-icon.png" width="30" height="30" alt="">
      <a class="navbar-brand" href="#">Remote Lab: {{labName}}</a>
      <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
      <span class="navbar-toggler-icon"></span>
      </button>

      <div class="collapse navbar-collapse" id="navbarSupportedContent">
          <ul class="navbar-nav me-auto mb-2 mb-lg-0">
              <li class="nav-item dropdown">
                  <a class="nav-link dropdown-toggle" href="#" id="navbarDropdown" role="button" data-bs-toggle="dropdown" aria-expanded="false">
                   Menu
                  </a>
                  <ul class="dropdown-menu" aria-labelledby="navbarDropdown">
                    <li><a class="dropdown-item" href="#" @click='toggleComponent("snapshot")'>Data Snapshot</a></li>
                    <li v-if='getShowTheory'><a class="dropdown-item" href="#" @click='toggleComponent("theoreticalstrains")'>Theoretical Values</a></li>
                    <li><a class="dropdown-item" href="#" @click='toggleComponent("graph")'>Graph</a></li>
                  </ul>
              </li>

              <li class="nav-item dropdown">
                  <a class="nav-link dropdown-toggle" href="#" id="navbarDropdown2" role="button" data-bs-toggle="dropdown" aria-expanded="false">
                   Tools
                  </a>
                  <ul class="dropdown-menu" aria-labelledby="navbarDropdown2">
                    <li><a class="dropdown-item" href="#" @click='addTool("ruler")'>Ruler</a></li>
                    <li><a class="dropdown-item" href="#" @click='addTool("protractor")'>Protractor</a></li>
                  </ul>
              </li>

              <li class="nav-item">
                  <a class="nav-link" href="#" tabindex="-1" @click='clearWorkspace'>Clear Workspace</a>
              </li>

              <li class="nav-item dropdown">
                  <a class="nav-link dropdown-toggle" href="#" id="navbarDropdown2" role="button" data-bs-toggle="dropdown" aria-expanded="false">
                   Layout
                  </a>
                  <ul class="dropdown-menu" aria-labelledby="navbarDropdown2">
                    <li><a class="dropdown-item" href="#" @click='toggleLayout(1)'>Single Column</a></li>
                    <li><a class="dropdown-item" href="#" @click='toggleLayout(0.25)'>25%:75%</a></li>
                    <li><a class="dropdown-item" href="#" @click='toggleLayout(0.5)'>50%:50%</a></li>
                    <li><a class="dropdown-item" href="#" @click='toggleLayout(0.75)'>75%:25%</a></li>
                  </ul>
              </li>
              
          </ul>

          <div class='d-flex'>
            <toolbar class='me-5' parentCanvasID="" parentDivID="navbar" parentComponentName="navbar" :showDownload="false" :showOptions="false" :showPopupHelp="true">
                  <template v-slot:popup>
                    <div class='row'>
                      <div class='col-6'>
                        <h2> Layout </h2>
                        <p> The layout of the UI can be changed using the Layout dropdown menu. This changes the left and right screen column sizes.</p>
                        <p>Components can be swapped by dragging to new positions. Click and drag from near but within the dotted line of the component you want to move. Release 
                          inside the white background of the dotted border that you want to move it to.</p>
                      </div>
                      <div class='col-6'>
                        <h2>UI Control:</h2>
                        <p>When the Measuring Tools are added hold, 'o' whilst dragging a tool to rotate it</p>
                        <p>Press 'w' to swap between controlling the measuring tools and the background UI. Click 'Clear Workspace' to remove the measuring tools</p>
                        <p>Additional UI components can be added from the Menu bar.</p>
                      </div>
                    </div>
                    <div v-if='!getShowForce' class='row'>
                      <div class='col-12'>
                        <h2>Hidden force values</h2>
                        <p>In this version of the UI, load force values have been hidden. Users should calculate the load force necessary to generate the measured strains</p>
                      </div>
                    </div>
                  </template>
            </toolbar>
          </div>


          <div class='d-flex'>
              <clock />
          </div>

      </div>
    </div>
  </nav>

</template>

<script>

import Clock from "./Clock.vue";
import Toolbar from './elements/Toolbar.vue';
import { mapGetters } from 'vuex';

export default {

  name: 'NavigationBar',
  components: {
    Clock,
    Toolbar
  },
  props:{
      
  },
  emits:[
    'togglegraph', 'togglesnapshot', 'toggleworkspace', 'clearworkspace', 'addruler', 'addprotractor', 'toggletheoreticalstrains', 'togglelayout'
  ],
  data () {
    return {
        
    }
  },
  computed:{
    ...mapGetters([
      'getShowTheory',
      'getShowForce'
    ]),
      labName(){
        return 'Truss Lab';
      }
  },
  methods: {
      addTool(tool){
          this.toggleComponent('workspace');
          setTimeout(() => {this.$emit('add' + tool)}, 100);  //give the workspace time to initialise and then send tool event
      },
      toggleComponent(component){
          this.$emit('toggle' + component);
      },
      clearWorkspace(){
          this.$emit('clearworkspace');
      },
      toggleLayout(ratio){
        if(ratio == 0.25)
        {
          this.$emit('togglelayout', 0.25);
        }
        else if(ratio == 0.5)
        {
          this.$emit('togglelayout', 0.5);
        }
        else if(ratio == 0.75)
        {
          this.$emit('togglelayout', 0.75);
        }
        else 
        {
          this.$emit('togglelayout', 1);
        }
      }
  }
}
</script>

<style scoped>


</style>