
<template>
<div class="container-fluid m-2 background-white border rounded" id='snapshot'>
    <div class="row table" id='table' >

        <table>
            <thead class='table-head background-primary'>
                <tr class='background-primary text-white'>
                    <th v-for='heading in headings' :key="heading" scope="col">{{heading}}</th>
                </tr>
            </thead>
            <tr v-for="(row, index) in getHistory" :id="'history-' + index" :key="index">
                <td v-for='key in Object.keys(row)' :key="key">{{key == 'load_cell' && !getShowForce ? '' : row[key].toFixed(0)}}</td>
            </tr>

            <tr :class='getLiveClass' id="current-data" key="current-data">
                <td v-for='key in Object.keys(getCurrent)' :key="key">{{key == 'load_cell' && !getShowForce ? '' : getCurrent[key].toFixed(0)}}</td>
            </tr>
                   

            <div id='table-bottom'>
                <div class='white-divider'>
                </div>
            </div> 

        </table> 

	</div>


    <div class='d-grid gap-2 d-sm-block'>
        <button id="snapshot-button" type='button' class="button-sm button-primary" @click="takeSnapshot(); scrollTo('table')">Record Snapshot</button>
        <button id="reset-snaps" type='button' class="button-sm button-danger" @click="toggleResetModal">Reset</button>
        <button id="download-snaps" type='button' class="button-sm button-secondary" @click="outputToCSV">Download Snapshots</button>
    </div>
    
     <toolbar parentCanvasID="" parentComponentName="snapshot" parentDivID="motor-snapshot-div" :showDownload='false' :showPopupHelp="true" :showOptions="false">  
        <template v-slot:popup>
            <div class='row mb-2'>
                <div class='col'>
                    <h3> Snapshot tool </h3>
                    <p> Click 'Record Snapshot' to save the current data. Every time you click a new data set will be added. This data is also displayed on the Graph component 
                        if added to the UI from the Menu. Click 'Download Snapshots' to download all the snapshots as a .csv file.
                    </p>
                </div>
            </div>

        </template>
    </toolbar>

    <div v-if='showResetConfirmModal' class="modal modal-show" tabindex="-1" role="dialog">
        <div class="modal-dialog" role="document">
          <div class="modal-content">
            <div class="modal-header">
              <h5 class="modal-title">Reset Data</h5>
              <button type="button" class="btn btn-close" data-bs-dismiss="modal" aria-label="Close" @click='toggleResetModal'>
                
              </button>
            </div>
            <div class="modal-body">
              <p>Are you sure you want to Reset? This will clear all stored data.</p>
            </div>
            <div class="modal-footer">
              <button type="button" class="button-sm button-danger" @click="resetSnaps(); toggleResetModal();">Reset</button>
              <button type="button" class="button-sm button-primary" data-dismiss="modal" data-bs-dismiss="modal" @click="toggleResetModal">Cancel</button>
            </div>
          </div>
        </div>
      </div>

</div>
</template>

<script>
import { mapGetters } from 'vuex';
import Toolbar from "./elements/Toolbar.vue"

export default {
    name: 'Snapshot',
    props:[
      'headings'
    ],
    components:{
        Toolbar,
    },
    data(){
        return{
            showResetConfirmModal: false
        }
    },
    computed:{
        ...mapGetters([
            'getCurrent',
            'getHistory',
            'getSmState',
            'getShowForce'
        ]),
        getLiveClass(){
            if(this.getSmState == 1){
                return 'current border'
            } else{
                return 'not-updating'
            }
        }
        
    },
    methods: {
        takeSnapshot(){
            let data = this.getCurrent;
            this.$store.dispatch('addToHistory', data);
        },
        resetSnaps(){
            this.$store.dispatch('clearAllHistory');
        },
        toggleResetModal(){
            this.showResetConfirmModal = !this.showResetConfirmModal;
        },
        scrollTo(id){
            var element = document.getElementById(id);
            element.scrollTop = element.scrollHeight;

        },
        outputToCSV(){
          let csv = '';
          let filename = '';
          let _this = this;
            let date = new Date();
            filename = 'TRUSS_SNAPSHOTS_' + date.getDate().toString() + (date.getMonth() + 1).toString() + date.getFullYear().toString();
          
            if(_this.getShowForce){
                    csv = 'Load/N, Gauge 1/µε , Gauge 2/µε,Gauge 3/µε, Gauge 4/µε, Gauge 5/µε, Gauge 6/µε\n';
                } else{
                    csv = 'Gauge 1/µε , Gauge 2/µε,Gauge 3/µε, Gauge 4/µε, Gauge 5/µε, Gauge 6/µε\n';
                }
              
            this.getHistory.forEach(function(d){
                if(_this.getShowForce){
                    csv += d.load_cell.toString();
                    csv += ",";
                } 
                csv += d.gauge_1.toString();
                csv += ',';
                csv += d.gauge_2.toString();
                csv += ',';
                csv += d.gauge_3.toString();
                csv += ',';
                csv += d.gauge_4.toString();
                csv += ',';
                csv += d.gauge_5.toString();
                csv += ',';
                csv += d.gauge_6.toString();
                     
                csv += "\n";
            });
            filename += '.csv';
          
          let hiddenElement = document.createElement('a');
          hiddenElement.href = 'data:text/csv;charset=utf-8,' + encodeURI(csv);
          hiddenElement.target = '_blank';
          hiddenElement.download = filename;
          hiddenElement.click();
      },
      },
      
}
</script>

<style scoped>

.current{
    border: thin;
	box-shadow: 0px 0px;
    color: green
}

.not-updating{
    color: rgb(169, 189, 169)
}

.h-divider{
 margin-top:5px;
 margin-bottom:5px;
 height:1px;
 width:100%;
 border-top:1px solid gray;
}

.v-divider{
 margin-left:5px;
 margin-right:5px;
 padding: 0px;
 width:1px;
 height:100%;
 border-right:1px solid gray;
}

.modal-show{
    display: block;
}
</style>
