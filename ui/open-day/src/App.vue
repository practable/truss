<template>
  <div id="app" class='container-fluid-sm m-0'>
    
    <navigation-bar 
    :isGraphOn='isGraphOn' :isTheoreticalStrainsOn="isTheoreticalStrainsOn" :isSnapshotOn="isSnapshotOn"
    @togglegraph="togglegraph" @togglesnapshot="toggleSnapshot" @toggletheoreticalstrains="toggleTheoreticalStrains" @toggleworkspace="addWorkspace" @clearworkspace="clearWorkspace" @addruler="rulerAdded = true" @addprotractor="protractorAdded = true"
    />

    <div v-if="isWorkspaceOn">
          <workspace :protractorAdded="protractorAdded" :rulerAdded="rulerAdded"/>
        </div>

  <streams id='streams' />

  <div v-if='!isMobile' class='row' id='component-grid'>

      <div class='d-flex' id='first-row'>
          <div class='drop-area drop-area-two-fifths' id='drop_0_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><data-stream id='data-stream' /></div>
          <div v-if='loadControlVersion == 3' class='drop-area drop-area-three-fifths' id='drop_0_1' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><load-control-code id='load-control-code' /></div>
          <div v-else class='drop-area drop-area-three-fifths' id='drop_0_1' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><load-control id='load-control' :controlVersion="loadControlVersion"/></div>
      </div>

      <div class='d-flex' id='second-row'>
        <div class='drop-area drop-area-full' id='drop_1_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><snapshot v-if='isSnapshotOn' id='snapshot' :headings="[getLoadHeading, 'Gauge 1 [&mu;&epsilon;]', 'Gauge 2 [&mu;&epsilon;]', 'Gauge 3 [&mu;&epsilon;]', 'Gauge 4 [&mu;&epsilon;]', 'Gauge 5 [&mu;&epsilon;]', 'Gauge 6 [&mu;&epsilon;]' ]"/></div>
        <!-- <div class='drop-area drop-area-half' id='drop_1_1' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"></div> -->
      </div>

      <div class='d-flex' id='third-row'>
        <div class='drop-area drop-area-half' id='drop_2_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><graph-output v-if='isGraphOn' id='graph' /></div>
        <div class='drop-area drop-area-half' id='drop_2_1' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><theoretical-strains id='theoretical-strains' v-if='isTheoreticalStrainsOn && getShowTheory' /></div>
      </div>

  </div>

  <div v-else class='d-flex flex-column' id='component-grid'>
      <div class='drop-area drop-area-mobile' id='drop_0_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><data-stream id='data-stream' /></div>
      <div class='drop-area drop-area-mobile' id='drop_1_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><load-control id='load-control' :controlVersion="loadControlVersion"/></div>
      <div class='drop-area drop-area-mobile' id='drop_2_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><snapshot v-if='isSnapshotOn' id='snapshot' :headings="[getLoadHeading, 'Gauge 1 [&mu;&epsilon;]', 'Gauge 2 [&mu;&epsilon;]', 'Gauge 3 [&mu;&epsilon;]', 'Gauge 4 [&mu;&epsilon;]', 'Gauge 5 [&mu;&epsilon;]', 'Gauge 6 [&mu;&epsilon;]' ]"/></div>
      <div class='drop-area drop-area-mobile' id='drop_3_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><theoretical-strains id='theoretical-strains' v-if='isTheoreticalStrainsOn && getShowTheory' /></div>
      <div class='drop-area drop-area-mobile' id='drop_4_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"><graph-output v-if='isGraphOn' id='graph' /></div>
      <div class='drop-area drop-area-mobile' id='drop_5_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"></div>
      <div class='drop-area drop-area-mobile' id='drop_6_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"></div>
      <div class='drop-area drop-area-mobile' id='drop_7_0' :draggable='getDraggable' @dragstart="dragComponent" @drop='dropComponent' @dragover.prevent @dragenter='dragEnter' @dragleave="dragLeave"></div>
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
      loadControlVersion: 5,    //0 => input field, 1=> slider, 2=>simulation of physical knob, 3 => direct commands typed, 4 => GamePad, 5 => open day, simple buttons
      isSnapshotOn: true,
      isTheoreticalStrainsOn: false,
      isGraphOn: false,
      isWorkspaceOn: false,
      rulerAdded: false,
      protractorAdded: false,
      // leftClass: 'col-lg-6',
      // rightClass: 'col-lg-6' 
    }
  },
  created(){
    this.$store.dispatch('setUsesLocalStorage', this.hasStorage());
    this.updateUUID();
  },
  computed:{
    ...mapGetters([
      'getDraggable',
      'getShowTheory',
      'getShowForce',
      'getUsesLocalStorage'
    ]),
    getLoadHeading(){
      if(this.getShowForce){
        return 'Load[N]';
      } else{
        return '';
      }
    },
    isMobile(){
      if(window.screen.width < 992){
        return true;
      } else{
        return false;
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
         //console.log("Dragged event: ");
         //console.log(event);
         let element = event.target;
         if(element.classList.contains('drop-area')){
           if(element.childNodes[0] != null){
              event.dataTransfer.setData("text/html", element.id + "|" + element.childNodes[0].id);
           } else{
              event.dataTransfer.setData("text/html", element.id + "|" + 'empty');
           }
            
         } else{
           while(element.parentNode){
              element = element.parentNode;
              //console.log(element.id);
              if(element.classList.contains('drop-area')){
                if(element.childNodes[0] != null){
                  event.dataTransfer.setData("text/html", element.id + "|" + element.childNodes[0].id);
              } else{
                  event.dataTransfer.setData("text/html", element.id + "|" + 'empty');
              }
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
      
      // only try if the dragged element is not empty
      if(draggedID != 'empty' && document.getElementById(draggedID) != null){
          if(droppedElement != null && droppedElement.classList.contains('drop-area')){
            if(event.target.childNodes.length > 0){
              draggedZone.appendChild(event.target.childNodes[0]);
            }
            //console.log(draggedID);
            droppedElement.appendChild(document.getElementById(draggedID));
            droppedElement.classList.remove('drop-area-highlighted');
        } 
        else if(droppedElement){
          let element = droppedElement;
          while(element.parentNode){
            element = element.parentNode;
            if(element.classList.contains('drop-area')){
              //console.log(element.childNodes[0]);
              draggedZone.appendChild(element.childNodes[0]);
              element.appendChild(document.getElementById(draggedID));
              element.classList.remove('drop-area-highlighted');
              break;
            }
          }
        }
      } else{
        if(droppedElement != null){
          droppedElement.classList.remove('drop-area-highlighted');
        }
        
      }
      
      return false;
    },
    dragEnter(event){
    //console.log(event)
    if(event.target.classList.contains('drop-area')){
          event.target.classList.add('drop-area-highlighted');
        }
    },
    dragLeave(event){
      let element = document.getElementById(event.target.id);
      if(element != null){
        element.classList.remove('drop-area-highlighted');
      }
      
    },
    hasStorage(){
        try {
            window.localStorage.setItem('test', 'storage');
            window.localStorage.removeItem('test');
            return true;
        } catch (exception) {
            return false;
          }
      },
      // UUID is generated by the practable booking system and stored in localStorage (as userName)
      // This function should just find that userName and set it in vuex, else set uuid to null
    updateUUID(){
        let stored_uuid;
        if(this.getUsesLocalStorage){
          stored_uuid = window.localStorage.getItem('userName');
        } else {
          stored_uuid = null;
        }
        
        if(stored_uuid){
            this.$store.dispatch('setUUID', stored_uuid);
        } else{
          this.$store.dispatch('setUUID', 'null');
        }
      },
    // toggleLayout(ratio){
    //   if(ratio == 0.25){
    //     this.leftClass = 'col-lg-3';
    //     this.rightClass = 'col-lg-9';
    //   } else if(ratio == 0.5){
    //     this.leftClass = 'col-lg-6';
    //     this.rightClass = 'col-lg-6';
    //   } else if(ratio == 0.75){
    //      this.leftClass = 'col-lg-9';
    //     this.rightClass = 'col-lg-3';
    //   } else{
    //      this.leftClass = 'col-lg-12';
    //     this.rightClass = 'col-lg-12';
    //   }
    // },
    
  }
  
}
</script>

<style>

</style>
