//Updated for Vue3, removing eventBus $on waiting for addruler/protractor event - now controlled through props from App.vue

<template>
<div>
    <canvas :class="workspace_canvas_clickable ? 'clickable' : 'unclickable'" id="workspace" @mousedown="checkClick" @mousemove="moveClicked" @mouseup="mouseUnclick"></canvas>
    <img id="protractor" src="/images/protractor.png" hidden>
    <img id="ruler" src="/images/ruler.png" hidden>
</div>
</template>

<script>

var canvas;
var ctx;
let protractor = new Image();
let ruler = new Image();
let shapes = [];        //the added objects to canvas

export default {
    name: "Workspace",
    props:{
      protractorAdded: Boolean,
      rulerAdded: Boolean
  },
    data(){
        return{
            isSelected: false,
            selected_index: null,
            selected_offset_x: 0,
            selected_offset_y: 0,
            rotateMode: false,
            workspace_canvas_clickable: true,
            ruler_width: 800,
            ruler_height: 80,
            ruler_ratio: 0.1,
            image_canvas: null,
        }
    },
    created(){
        this.image_canvas = document.getElementById('truss-image');

        if(this.image_canvas == null){
            this.image_canvas = document.getElementById('session-end-image');
        }
    },
    mounted(){
        shapes = [];        //ensure when mounted again that the shapes are not redrawn
        canvas = document.getElementById("workspace");
        canvas.width = screen.width;
        canvas.height = screen.height;
        ctx = canvas.getContext("2d");

        //add a key press modifiers to the window
        window.addEventListener('keydown', this.updateMode, false);
        window.addEventListener('keyup', this.updateMode, false);
        window.addEventListener('resize', () => {setTimeout(this.resizeRuler, 100)});

        this.resizeRuler();
    },
    watch:{
        protractorAdded(set) {
            if(set){
                this.addProtractor();
            }
        },
        rulerAdded(set) {
            if(set){
                this.addRuler();
            }
        },
    },
    methods:{
        draw() {
            ctx.clearRect(0,0,screen.width, screen.height);

            for(let i=0; i< shapes.length;i++){

                ctx.save();

                if(shapes[i].image != null){
                    if(shapes[i].image == ruler){
                        shapes[i].width = this.ruler_width;
                        shapes[i].height = this.ruler_height;
                    } 
                    ctx.translate(shapes[i].x,shapes[i].y);
                    ctx.translate(shapes[i].width/2, shapes[i].height/2)
                    ctx.rotate(shapes[i].angle);
                    //ctx2.drawImage(shapes[i].image, shapes[i].x, shapes[i].y, shapes[i].width, shapes[i].height);
                    ctx.drawImage(shapes[i].image, -shapes[i].width/2, -shapes[i].height/2, shapes[i].width, shapes[i].height);
                } 
                else
                {
                    // ctx2.clearRect(0,0,screen.width, screen.height);
                    ctx.beginPath();
                    ctx.rect(shapes[i].x, shapes[i].y, shapes[i].width, shapes[i].height);
                    ctx.stroke();
                    ctx.fill();
                    ctx.fillStyle = "#444444";
                }

                ctx.restore();
            }

        },
        addProtractor(){
            protractor.onload = function() {
                let x = 100;
                let y= 100;
                let w=400;
                let h=200;
                shapes.push( {x:x, y:y, width:w, height:h, image:protractor, angle:0} );
                ctx.drawImage(protractor, x, y, w, h);
                
                
            };
            protractor.src = document.getElementById("protractor").src;
        },
        addRuler(){
            let _this = this;
            ruler.onload = () => {
                let x = 100;
                let y= 300;
                let w = _this.ruler_width;
                let h = _this.ruler_height;
                
                
                shapes.push( {x:x, y:y, width:w, height:h, image:ruler, angle:0} );
                ctx.drawImage(ruler, x, y, w, h);
                
            };
            ruler.onerror = function(){
                console.log('no image');
            }

            ruler.src = document.getElementById("ruler").src;
        },
        updateMode(event){
            if(event.repeat){
                return;
            } else{
                if(event.key == "o"){
                    this.rotateMode = !this.rotateMode;
                } 
                else if(event.key == "w" && event.type == 'keydown'){
                    console.log(event);
                    this.workspace_canvas_clickable = !this.workspace_canvas_clickable;
                }
            }
            
        },
        checkClick(event){
            for(let i=0; i<shapes.length;i++){

                let origin = [0,0];
                let centre = [shapes[i].x + shapes[i].width/2, shapes[i].y + shapes[i].height/2];

                //click position relative to the centre of the shape.
                let clickPos = [event.offsetX - centre[0], event.offsetY - centre[1]];
                //let clickPos = [event.offsetX, event.offsetY];

                //from the centre of the shape
                let vertices = [];
                let vertex1 = [shapes[i].x - centre[0], shapes[i].y - centre[1]];
                let vertex2 = [shapes[i].x + shapes[i].width - centre[0], shapes[i].y - centre[1]];
                let vertex3 = [shapes[i].x + shapes[i].width - centre[0], shapes[i].y + shapes[i].height - centre[1]];
                let vertex4 = [shapes[i].x - centre[0], shapes[i].y + shapes[i].height - centre[1]];
                vertices.push(vertex1);
                vertices.push(vertex2);
                vertices.push(vertex3);
                vertices.push(vertex4);

                let angle = shapes[i].angle;

                //rotate the vertices around the centre of the shape
                for(let i=0;i<4;i++){
                    let x_prev = vertices[i][0];
                    let y_prev = vertices[i][1];

                    vertices[i][0] = x_prev*Math.cos(angle) - y_prev*Math.sin(angle);
                    vertices[i][1] = x_prev*Math.sin(angle) + y_prev*Math.cos(angle);
                }
                
                if(!this.doIntersect(origin,clickPos,vertices[0], vertices[1]) && !this.doIntersect(origin,clickPos,vertices[1], vertices[2]) && !this.doIntersect(origin,clickPos,vertices[2], vertices[3]) && !this.doIntersect(origin,clickPos,vertices[3], vertices[0])){
                    this.isSelected = true;
                    this.selected_index = i;
                    this.selected_offset_x = event.offsetX - shapes[i].x;
                    this.selected_offset_y = event.offsetY - shapes[i].y;
                }

            }
            
        },
        // To find orientation of ordered triplet (p, q, r). 
        // The function returns following values 
        // 0 --> p, q and r are colinear 
        // 1 --> Clockwise 
        // 2 --> Counterclockwise 
        checkOrientation(q, p, r){
            // See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
                // for details of below formula. 
                let val = (q[1] - p[1]) * (r[0] - q[0]) - 
                        (q[0] - p[0]) * (r[1] - q[1]); 
            
                if (val == 0) return 0;  // colinear 
            
                return (val > 0) ? 1: 2; // clock or counterclock wise 
        },
        doIntersect(p1,q1,p2,q2){
            let o1 = this.checkOrientation(p1, q1, p2); 
            let o2 = this.checkOrientation(p1, q1, q2); 
            let o3 = this.checkOrientation(p2, q2, p1); 
            let o4 = this.checkOrientation(p2, q2, q1); 
  
            // General case 
            if (o1 != o2 && o3 != o4) {
                return true;
            } else{
                return false;
            }
                 
        },
        moveClicked(event){
            if(this.isSelected){
                if(this.rotateMode){
                    if(event.movementY < 0){
                        shapes[this.selected_index].angle -= Math.PI/180;
                    } else if(event.movementY > 0){
                        shapes[this.selected_index].angle += Math.PI/180;
                    }
                    
                } 
                else
                {
                    //ctx2.clearRect(rect_top_left_x, rect_top_left_y, rect_width, rect_height);
                    shapes[this.selected_index].x = event.offsetX - this.selected_offset_x;
                    shapes[this.selected_index].y = event.offsetY - this.selected_offset_y;
                
                }
                this.draw();        //redraw the shapes only when attempting to move
            }
            
        },
        mouseUnclick(){
            this.isSelected = false;
            this.selected_index = null;
        },
        resizeRuler(){
            this.ruler_width = this.image_canvas.clientWidth;
            this.ruler_height = this.ruler_ratio*this.ruler_width;
            this.draw();
        },
    }
}


</script>

<style scoped>

#workspace{
    z-index: 999;
    position:absolute;
    left:0px;
    top:100px;
    /* pointer-events: none; */
}

.unclickable{
    pointer-events: none;
    opacity: 0.5;
}

.clickable{
    pointer-events: all;
    opacity: 1;
}


</style>