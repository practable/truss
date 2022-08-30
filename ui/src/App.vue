<template>
  <div id="app" class='container-fluid-sm m-0 background-grey'>
    <navigation-bar @togglelayout="toggleLayout" @togglegraph="togglegraph" @togglesnapshot="toggleSnapshot" @toggletheoreticalstrains="toggleTheoreticalStrains" @toggleworkspace="addWorkspace" @clearworkspace="clearWorkspace" @addruler="rulerAdded = true" @addprotractor="protractorAdded = true"
    />

    <div v-if="isWorkspaceOn">
          <workspace :protractorAdded="protractorAdded" :rulerAdded="rulerAdded"/>
        </div>

  <streams id='streams' />

  <!-- <consent /> -->
  <!-- <watcher /> -->

     <div class='row' id='component-grid'>

          <div :class='leftClass' id='left-screen'>
            <div class='col drop-area' id='drop_0_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent><data-stream id='data-stream' /></div>
            <div class='col drop-area' id='drop_1_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent><snapshot v-if='isSnapshotOn' id='snapshot' :headings="[getLoadHeading, 'Gauge 1/ &mu;&epsilon;', 'Gauge 2/ &mu;&epsilon;', 'Gauge 3/ &mu;&epsilon;', 'Gauge 4/ &mu;&epsilon;', 'Gauge 5/ &mu;&epsilon;', 'Gauge 6/ &mu;&epsilon;' ]"/></div>
            <div class='col drop-area' id='drop_2_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent><graph-output v-if='isGraphOn' id='graph' /></div>
            <div class='col drop-area' id='drop_3_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent></div>
            <div class='col drop-area' id='drop_4_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent></div>
          </div>

          <div :class='rightClass' id='right-screen'>
            <div v-if='loadControlVersion == 3' class='col drop-area' id='drop_0_1' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent><load-control-code id='load-control-code' /></div>
            <div v-else class='col drop-area' id='drop_0_1' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent><load-control id='load-control' :controlVersion="loadControlVersion"/></div>

            <div class='col drop-area' id='drop_1_1' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent><theoretical-strains id='theoretical-strains' v-if='isTheoreticalStrainsOn && getShowTheory' /></div>
            <div class='col drop-area' id='drop_2_1' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent></div>
            <div class='col drop-area' id='drop_3_1' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent></div>
            <div class='col drop-area' id='drop_4_1' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter.prevent></div>
          </div>

      </div>

  
  </div>
</template>

<script>

import LoadControl from './components/LoadControl.vue';
import LoadControlCode from './components/LoadControlCode.vue';
import TheoreticalStrains from './components/TheoreticalStrains.vue';
import NavigationBar from './components/NavigationBar.vue';
import Snapshot from './components/Snapshot.vue';
import GraphOutput from './components/GraphOutput.vue';
import Workspace from './components/Workspace.vue';
import Streams from './components/Streams.vue';
import DataStream from './components/DataStream.vue';


import { mapGetters } from 'vuex';

export default {
  name: 'App',
  components: {
    NavigationBar,
    LoadControl,
    LoadControlCode,
    TheoreticalStrains,
    Snapshot,
    GraphOutput,
    Workspace,
    Streams,
    DataStream,
  },
  data(){
    return {
      loadControlVersion: 1,    //0 => input field, 1=> slider, 2=>simulation of physical knob, 3 => direct commands typed, 4 => GamePad
      isSnapshotOn: true,
      isTheoreticalStrainsOn: false,
      isGraphOn: false,
      isWorkspaceOn: false,
      rulerAdded: false,
      protractorAdded: false,
      leftClass: 'col-lg-6',
      rightClass: 'col-lg-6' 
    }
  },
  computed:{
    ...mapGetters([
      'getDraggable',
      'getShowTheory',
      'getShowForce'
    ]),
    getLoadHeading(){
      if(this.getShowForce){
        return 'Load/N';
      } else{
        return '';
      }
    }

  },
  methods:{
    togglegraph(){
      console.log('toggle graph');
      this.isGraphOn = !this.isGraphOn;
    },
    toggleSnapshot(){
      console.log('toggle snapshot');
      this.isSnapshotOn = !this.isSnapshotOn;
    },
    toggleTheoreticalStrains(){
      console.log('toggle theoretical strains');
      this.isTheoreticalStrainsOn = !this.isTheoreticalStrainsOn;
    },
    addWorkspace(){
      this.isWorkspaceOn = true;
    },
    clearWorkspace(){
      this.isWorkspaceOn = false;
    },
    dragComponent(event){
        event.dataTransfer.effectAllowed = 'move';
         console.log(event.target.id);
         let element = event.target;
         if(element.classList.contains('drop-area')){
           console.log(element.id);
            event.dataTransfer.setData("text/html", element.id + "|" + element.childNodes[0].id);
            console.log(element.childNodes[0]);
         } else{
           while(element.parentNode){
              element = element.parentNode;
              console.log(element.id);
              if(element.classList.contains('drop-area')){
                event.dataTransfer.setData("text/html", element.id + "|" + element.childNodes[0].id);
                console.log(element.childNodes[0]);
                break;
              }
            }
         }
    },
    dropComponent(event){
      event.preventDefault();
      event.stopPropagation();
      let dropData = event.dataTransfer.getData('text/html');
      let dropItems = dropData.split("|");
      let draggedZone = document.getElementById(dropItems[0]);
      let droppedElement = document.getElementById(event.target.id);
      let draggedID = dropItems[1];
      
      if(droppedElement != null && droppedElement.classList.contains('drop-area')){
        if(event.target.childNodes.length > 0){
          draggedZone.appendChild(event.target.childNodes[0]);
        }
        console.log(draggedID);
        droppedElement.appendChild(document.getElementById(draggedID));
      } 
      else if(droppedElement){
        let element = droppedElement;
        while(element.parentNode){
          element = element.parentNode;
          if(element.classList.contains('drop-area')){
            console.log(element.childNodes[0]);
            draggedZone.appendChild(element.childNodes[0]);
            element.appendChild(document.getElementById(draggedID));
            
            break;
          }
        }
      }
      return false;
    },
    toggleLayout(ratio){
      if(ratio == 0.25){
        this.leftClass = 'col-lg-3';
        this.rightClass = 'col-lg-9';
      } else if(ratio == 0.5){
        this.leftClass = 'col-lg-6';
        this.rightClass = 'col-lg-6';
      } else if(ratio == 0.75){
         this.leftClass = 'col-lg-9';
        this.rightClass = 'col-lg-3';
      } else{
         this.leftClass = 'col-lg-12';
        this.rightClass = 'col-lg-12';
      }
    },
    
  }
  
}
</script>

<style>

</style>
