//Vue3 updated

<template>
<div class='container-sm m-2 bg-white rounded'>
<div class='row' id="video">
	<div class='col-12'>
		<video-element :url="url" />
	</div>
</div>
</div>
</template>

<script>
import axios from "axios";
import VideoElement from "./VideoElement.vue";
import { mapGetters } from 'vuex';

export default {
	name: "WebcamStream",
	components:{
		VideoElement,
	},
    data(){
        return{
			stream: Object,
        }
    },
    computed:{
		...mapGetters({
			url: 'getVideoURL',
			urlOK: 'getVideoURLObtained'
		}),
		streamOK(){			
			return this.$store.getters.getStream("video");
		},
		
		
	},
	mounted(){
		// var _this = this;
		// var reconnect = function () {
		// 	_this.accessVideo();
		// };
		//make second and subsequent connections
		document.addEventListener("streams:dropped", this.accessVideo);
	},
	methods:{
		accessVideo(){
			this.stream = this.$store.getters.getStream("video");
				var accessURL = this.stream.url;
				var token = this.stream.token;
				var store = this.$store;
				store.dispatch("deleteVideoURL");		////THIS HAS BEEN ADDED
				axios
				.post(accessURL, {}, { headers: { Authorization: token } })
				.then((response) => {
					store.dispatch("setVideoURL", response.data.uri);
				})
				.catch((err) => console.log(err));
		}
	},
	watch:{
		streamOK: function(is) {
			if (is) {
				this.accessVideo();
			} else{
				console.log("no stream");
			}
    },
		urlOK(is) {
			if (is) {
				console.log("get videoURL", this.urlOK, this.url);
			} 
		},

	}
}


</script>

<style scoped>
/* #video-canvas{
	width:80%;
	height: 100%;
} */
</style>