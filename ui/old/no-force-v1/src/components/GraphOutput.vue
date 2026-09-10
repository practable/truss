
<template>
<div class='container-fluid m-2 background-white border rounded'>
    <div class="row m-0 justify-content-center" id="chart-canvas">
        <div class="col-12">
            <canvas id='graph-canvas' @mousedown="startLine" @mouseup="endDrag" @mousemove="endLine"></canvas>
        </div>
    </div>

    <div class="row m-2 d-flex-sm" id="chart-functions" @mousedown="setDraggable(false)" @mouseup="setDraggable(true)">

        <div class='col-sm-6 flex-column'>
            <div class='row'>
                <div class='col-sm-6 dropdown'>
                    <button type="button" class="button-sm button-secondary dropdown-toggle" data-bs-toggle="dropdown" aria-expanded="false">
                        Y Axis
                    </button>
                    <ul class="dropdown-menu">
                        <li v-if='getShowForce'><a class="dropdown-item" @click='() => changeYAxis("load_cell")'>Load Cell</a></li>
                        <li><a class="dropdown-item" @click='() => changeYAxis("gauge_1")'>Member 1</a></li>
                        <li><a class="dropdown-item" @click='() => changeYAxis("gauge_2")'>Member 2</a></li>
                        <li><a class="dropdown-item" @click='() => changeYAxis("gauge_3")'>Member 3</a></li>
                        <li><a class="dropdown-item" @click='() => changeYAxis("gauge_4")'>Member 4</a></li>
                        <li><a class="dropdown-item" @click='() => changeYAxis("gauge_5")'>Member 5</a></li>
                        <li><a class="dropdown-item" @click='() => changeYAxis("gauge_6")'>Member 6</a></li>
                    </ul>
                </div>

                <div class='col-sm-6 dropdown'>
                    <button type="button" class="button-sm button-secondary dropdown-toggle" data-bs-toggle="dropdown" aria-expanded="false">
                        X Axis
                    </button>
                    <ul class="dropdown-menu">
                        <li v-if='getShowForce'><a class="dropdown-item" @click='() => changeXAxis("load_cell")'>Load Cell</a></li>
                        <li><a class="dropdown-item" @click='() => changeXAxis("gauge_1")'>Member 1</a></li>
                        <li><a class="dropdown-item" @click='() => changeXAxis("gauge_2")'>Member 2</a></li>
                        <li><a class="dropdown-item" @click='() => changeXAxis("gauge_3")'>Member 3</a></li>
                        <li><a class="dropdown-item" @click='() => changeXAxis("gauge_4")'>Member 4</a></li>
                        <li><a class="dropdown-item" @click='() => changeXAxis("gauge_5")'>Member 5</a></li>
                        <li><a class="dropdown-item" @click='() => changeXAxis("gauge_6")'>Member 6</a></li>
                    </ul>
                </div>
            </div>
            
            <div>
                <label class='txt-primary txt-bold m-2' for="gradient">Gradient:</label>
                <input type='text' class='input' id="gradient" :value="gradient.toFixed(2)" readonly > 
            </div>
        </div>
       
        
        <div class='col-sm-6 flex-column'>
            <label class='txt-primary txt-bold m-2' for="graph">Plot function: </label>
            <select class='button-sm button-secondary col-sm-6' name="function" id="function" v-model="currentFunction">
                <option value="linear">Linear</option>
                <option value="quadratic">Quadratic</option>
                <option value="trigonometric">Trigonometric</option>
                <option value="exponential">Exponential</option>
            </select> 

            <div v-if="currentFunction === 'linear'">
               
                <div class='row justify-content-center'>
                    <img id='linear_function' src='/images/LinearFunction.png'>
                </div>


                <div class='row justify-content-center'>
                    <div>
                        <label class='txt-primary m-2' for="func_a">a = </label>
                        <input type='number' class='input col-sm-3' id="func_a" v-model="func_a" size="3">
                    </div>
                    

                    <div>
                        <label class='txt-primary m-2' for="func_b">b = </label>
                        <input type='number' class='input col-sm-3' id="func_b" v-model="func_b" size="3">                
                    </div>
                </div>


                <div class="row-sm justify-content-center">
                    <button class="button-sm button-primary m-1" id="plotFunctionButton" @click="plotFunc(linear)">Plot</button>
                    <button class="button-sm button-danger m-1" id="clearFunctionButton" @click="deleteFunctionDataset">Clear</button>
                </div>
            </div>
                <div v-else-if="currentFunction === 'quadratic'">

                    <div class='row justify-content-center'>
                    <img id='linear_function' src='/images/QuadraticFunction.png'>
                </div>


                <div class='row justify-content-center'>
                    <div>
                        <label class='txt-primary m-2' for="func_a">a = </label>
                        <input type='number' class='input col-sm-3' id="func_a" v-model="func_a" size="3">
                    </div>
                    

                    <div>
                        <label class='txt-primary m-2' for="func_b">b = </label>
                        <input type='number' class='input col-sm-3' id="func_b" v-model="func_b" size="3">                
                    </div>
                </div>

                <div class="row-sm justify-content-center">
                    <button class="button-sm button-primary m-1" id="plotFunctionButton" @click="plotFunc(quadratic)">Plot</button>
                    <button class="button-sm button-danger m-1" id="clearFunctionButton" @click="deleteFunctionDataset">Clear</button>
                </div>
            </div>
            <div v-else-if="currentFunction === 'trigonometric'">

                <div class='row justify-content-center'>
                    <img id='trig_function' src='/images/TrigFunction.png'>
                </div>


                <div class='row justify-content-center'>
                    <div>
                        <label class='txt-primary m-2' for="func_a">A</label>
                        <input type='number' class='input col-sm-3' id="func_a" v-model="func_a" size="3">
                    </div>
                    
                    <div>
                        <label class='txt-primary m-2' for="func_b">&omega;</label>
                        <input type='number' class='input col-sm-3' id="func_b" v-model="func_b" size="3">
                    </div>

                    <div>
                        <label class='txt-primary m-2' for="func_c">&phi;</label>
                        <input type='number' class='input col-sm-3' id="func_c" v-model="func_c" size="3">                
                    </div>
                </div>

                <div class="row-sm justify-content-center">
                    <button class="button-sm button-primary m-1" id="plotFunctionButton" @click="plotFunc(trigonometric)">Plot</button>
                    <button class="button-sm button-danger m-1" id="clearFunctionButton" @click="deleteFunctionDataset">Clear</button>
                </div>
            </div>
            <div v-else-if="currentFunction === 'exponential'">

                 <div class='row justify-content-center'>
                    <img id='linear_function' src='/images/ExpFunction.png'>
                </div>


                <div class='row justify-content-center'>
                    <div>
                        <label class='txt-primary m-2' for="func_a">A = </label>
                        <input type='number' class='input col-sm-3' id="func_a" v-model="func_a" size="3">
                    </div>
                    

                    <div>
                        <label class='txt-primary m-2' for="func_b">b = </label>
                        <input type='number' class='input col-sm-3' id="func_b" v-model="func_b" size="3">                
                    </div>
                </div>

                <div class="row-sm justify-content-center">
                    <button class="button-sm button-primary m-1" id="plotFunctionButton" @click="plotFunc(exponential)">Plot</button>
                    <button class="button-sm button-danger m-1" id="clearFunctionButton" @click="deleteFunctionDataset">Clear</button>
                </div>
            </div>
            
        </div>

    </div>

    <toolbar parentCanvasID="graph-canvas" parentComponentName="graph" parentDivID="graph" :showDownload='true' :showPopupHelp="true" :showOptions="false">  
        
        <template v-slot:popup>
            <div class='row mb-2' id='intro-div'>
                <div class='col-12'>
                    <h3> Adding data </h3>
                    <p> The graph will display all data recorded by the Snapshot component. Change the displayed gauge values using the 'Y Axis' and 'X Axis' dropdowns. </p>
                </div>
            </div>

            <div class='row mb-2' id='gradient-div'>
                <div class='col-4'>
                    <img class='popup-image' src='/images/gradient.png'>
                </div>
                <div class='col-8'>
                    <h3> Gradient tool </h3>
                    <p> Click and drag on the graph in order to draw a straight line segment. The gradient of this line is displayed in the Gradient box.</p>
                </div>
            </div>

            <div class='row mb-2' id='data-point-div'>
                <div class='col-4'>
                    <img class='popup-image' src='/images/GraphDataPoint.png'>
                </div>
                <div class='col-8'>
                    <h3> Interactive data points </h3>
                    <p> Hover over a data point to get the exact values. Change the data plotted on each axis with the dropdown menus 'Y Axis' and 'X Axis'.
                    </p>
                </div>

            </div>

            <div class='row mb-2' id='functions-div'>
                <div class='col-4'>
                    <img class='popup-image' src='/images/function-plotting.png'>
                </div>
                <div class='col-8'>
                    <h3> Function Plotting </h3>
                    <p> Select the function type from the dropdown menu. Input the corresponding function parameters. Angular parameters are in radians. Click plot to display the function.
                        The function is plotted up to the maximum value on the x-axis.
                    </p>
                </div>

            </div>
        </template>
    </toolbar>
</div>

</template>

<script>

import { Chart } from 'chart.js';
import { mapActions, mapGetters } from 'vuex';
import Toolbar from "./elements/Toolbar.vue";

export default {
    
    name: 'GraphOutput',
    emits: ['newselectedgraphpoint'],
    components:{
        Toolbar,
    },
    data(){
        return{
            chart: null,
            gradient_start_point: {x:0, y:0},
            gradient_end_point: {x:0, y:0},
            gradient: 0,
            mouseDragging: false,
            currentFunction: "linear",
            func_a: 0,
            func_b: 0,
            func_c: 0,
            func_d:0,
            funcTimeStep: 1.0,
            YAxisMax: 0,
            YAxisMin: 0,
            XAxisMax: 0,
            XAxisMin: 0,
            maxDataPoints: 5000,
        }
    },
    mounted() {
        this.createChart();
        this.getAllData();
        //this.updateChart();
      },
    computed:{
        ...mapGetters([
            'getCurrent',
            'getHistory',
            'getNumData',
            'getYAxisVariable',
            'getXAxisVariable',
            'getXDataLatest',
            'getXDataAll',
            'getYDataLatest',
            'getYDataAll',
            'getShowForce'
        ]),
      },
    watch:{
        getHistory(){
            this.clearData();       //only runs when data array is completely changed, ie when it is set to empty array []
        },
        getNumData(){
            this.updateChart();
        }
    },
    methods: {
        ...mapActions([
            'setDraggable',
            'setXAxis',
            'setYAxis'
        ]),
        updateChart(){
            let max_index = this.getNumData - 1;
            if(max_index < this.maxDataPoints){
                this.getLatestData();
                this.chart.update(0);                       //actually updating the chart moved to here!
            } 
        },
        createChart() {
            var _this = this;
            const canvas = document.getElementById('graph-canvas');
            const ctx = canvas.getContext('2d');
            this.chart = new Chart(ctx, {
            type: 'scatter',
            data: {
                datasets: [{
                    label: 'graph',
                    data: [],
                    pointBackgroundColor: 'rgba(20, 51, 186, 1)',
                }]
            },
            options: {
                animation: false,
                parsing: false,
                legend:{
                    display: false
                },
                scales: {
                    xAxes: [{
                        scaleLabel:{
                            display: true,
                            labelString: _this.getXAxisVariable
                        },
                        type: 'linear',
                        position: 'bottom',
                        ticks: {
                            callback : (value,index,values) => {
                                _this.updateXAxisMax(value, index, values);
                                _this.updateXAxisMin(value, index);
                                return value;
                            },
                            
                        },
                        minRotation: 20,
                        maxRotation: 20,
                        sampleSize: 2,
                    }],
                    yAxes: [{
                        scaleLabel:{
                            display: true,
                            labelString: _this.getYAxisVariable
                        },
                        type: 'linear',
                        position: 'left',
                        ticks: {
                            callback : (value,index,values) => {
                                _this.updateYAxisMax(value, index);
                                _this.updateYAxisMin(value, index, values);
                                return value;
                            }
                        },
                        sampleSize: 2,
                    }],
                },
                responsive: true
            }
        });

            canvas.onclick = function(event){
                let active_points = _this.chart.getElementsAtEventForMode(event, 'index', { intersect: true }, false);
                if(active_points[0]){
                    _this.$emit('newselectedgraphpoint', active_points[0]._index);       //data point selected so send event to let other elements know.
                }
                
            };
        },
        updateYAxisMax(value, index){
            if(index == 0){
                this.YAxisMax = value;
            }
            
        },
        updateYAxisMin(value,index,values){
            if(index == values.length - 1){
                this.YAxisMin = value;
            }
        },
        updateXAxisMin(value, index){
            if(index == 0){
                this.XAxisMin = value;
            }
            
        },
        updateXAxisMax(value,index,values){
            if(index == values.length - 1){
                this.XAxisMax = value;
            }
        },
        addDataToChart(data) {
            this.chart.data.datasets[0].data.push(data);
        },
        clearData(){
            this.chart.destroy()
            this.createChart();
        },
        getAllData(){
            let x_data_all = this.getXDataAll;
            let y_data_all = this.getYDataAll;

            for(let i=0; i<this.getNumData;i++){
                
                let x_data = x_data_all[i];
                let y_data = y_data_all[i];
                this.addDataToChart({x: x_data, y: y_data});
                
            }

            this.chart.update(0);
        },
        getLatestData(){
            let index = this.getNumData - 1;
            if(index >= 0){
                let x_data = this.getXDataLatest;
                let y_data = this.getYDataLatest;
                
                this.addDataToChart({x: x_data, y: y_data});
            } 
        },
        getDataAtIndex(index){
            if(index >= 0 && index < this.getNumData){
                //console.log('getting data at: ' + index);
                let x_data = this.getXDataAll[index];
                let y_data = this.getYDataAll[index];
            
                this.addDataToChart({x: x_data, y: y_data});
            } else{
                console.log('could not get: ' + index);
            }
        },
        removeChart(){
            this.chart.destroy();
        },
        startLine(event){
            event.preventDefault();
            this.gradient_start_point.x = event.offsetX;
            this.gradient_start_point.y = event.offsetY;

            this.mouseDragging = true;
            
        },
        endDrag(){
            this.mouseDragging = false;
        },
        endLine(event){
            if(this.mouseDragging){
                //get ratio of y axis to x axis scales, get ratio of y difference in mouse positions and x difference in mouse positions.
                this.gradient_end_point.x = event.offsetX;
                this.gradient_end_point.y = event.offsetY;

                let pointer_ratio = (this.gradient_start_point.y - this.gradient_end_point.y) / (this.gradient_end_point.x - this.gradient_start_point.x);  //pointer ratio

                let canvas_offset = 32;         //might need to change/check this 
                let canvas = document.getElementById('graph-canvas');
                let canvas_height = canvas.clientHeight - canvas_offset;
                let canvas_width = canvas.clientWidth;
                let canvas_ratio = canvas_height/canvas_width;      //canvas ratio
                
                let y_diff = this.YAxisMax - this.YAxisMin;
                let x_diff = this.XAxisMax - this.XAxisMin;
                let axis_ratio = y_diff/x_diff;         //axis ratio

                if(this.getNumData > 1){
                    this.gradient = axis_ratio*pointer_ratio/canvas_ratio;
                    this.drawLine(this.gradient_start_point, this.gradient_end_point);
                }
            }
            
        },
        drawLine(from, to){
            
            //draw the gradient line
            //only draw anything if at least 2 data points have been plotted
            let canvas = document.getElementById('graph-canvas');
            const context = canvas.getContext('2d');
            context.clearRect(0, 0, canvas.width, canvas.height);
            this.chart.update(0);       //instantly update with no animation

            context.beginPath(); 
            // Staring point 
            context.moveTo(from.x, from.y);
            // End point 
            context.lineTo(to.x, to.y);
            context.lineWidth = 5;
            // set line color
            context.strokeStyle = 'red';
            // Make the line visible
            context.stroke();
            
        },
        plotFunc(func){
            // let min = store.getMinTime();
            // let max = store.getMaxTime();
            let min = this.XAxisMin;
            let max = this.XAxisMax;
            let t_delta = max-min;
            let num_plots = t_delta/this.funcTimeStep;
            let new_data = [];
            let new_t;
            let new_y;
            for(let i=0; i<num_plots;i++){
                new_t = min + i*this.funcTimeStep;
                new_y = func(new_t);
                let data = {
                    x: new_t,
                    y: new_y
                };
                new_data.push(data);
            }
            
            this.addNewDataSet('rgba(255, 0, 0, 0.5)', new_data);
        },
        linear(t){
            return (parseFloat(this.func_a)*t + parseFloat(this.func_b));
        },
        quadratic(t){
            return parseFloat(this.func_a)*t*t + parseFloat(this.func_b);
        },
        trigonometric(t){
            return parseFloat(this.func_a)*Math.sin(parseFloat(this.func_b)*t + parseFloat(this.func_c));
        },
        exponential(t){
            return parseFloat(this.func_a)*Math.exp(parseFloat(this.func_b)*t);
        },
        addNewDataSet(colour, data){
            this.chart.data.datasets.push({
                label:"plotted function",
                pointBackgroundColor: colour,
                data: data
                });
            this.chart.update(0);
        },
        deleteFunctionDataset(){
            this.chart.data.datasets = this.chart.data.datasets.filter(set => set.label !== "plotted function");
            this.chart.update(0);
        },
        changeYAxis(variable){
            this.setYAxis(variable);
            this.clearData();
            this.chart.options.scales.yAxes[0].scaleLabel.labelString = this.getYAxisVariable;
            this.getAllData();
            
        },
        changeXAxis(variable){
            this.setXAxis(variable);
            this.clearData();
            this.chart.options.scales.xAxes[0].scaleLabel.labelString = this.getXAxisVariable;
            this.getAllData();
            
        },

      },
      
}
</script>



<style scoped>

#linear_function{
    width: 120px;
    height: 30px;
}

#trig_function{
    width: 150px;
    height: 30px;
}



</style>
