import { mount } from '@vue/test-utils'
import { createStore } from 'vuex';
import ConnectAndDisplay from '../../src/components/ConnectAndDisplay.vue';
import dataStore from '../../src/modules/dataStore.js'
import uiStore from '../../src/modules/uiStore.js'
import streamStore from '../../src/modules/streamStore.js'

const createVuexStore = () => 

    createStore({
        modules:{
            data: dataStore,
            ui: uiStore,
            stream: streamStore
        }
    });


describe('ConnectAndDisplay', () => {

    
  test('can render with default store', () => {
    const store = createVuexStore();
    const wrapper = mount(ConnectAndDisplay, {
      global:{
        plugins: [store]
      }
    });

    expect(wrapper.find('#measured-values'));
  })

  test('displays gauge values', () => {
    const store = createVuexStore();
    store.dispatch('setCurrent', {load_cell: 10.0, gauge_1: 1, gauge_2:2, gauge_3:3, gauge_4:4, gauge_5:5, gauge_6:6});
    const wrapper = mount(ConnectAndDisplay, {
      global:{
        plugins: [store]
      }
    });

    expect(wrapper.find('#gauge-1').text()).toContain('1');
    expect(wrapper.find('#gauge-2').text()).toContain('2');
    expect(wrapper.find('#gauge-3').text()).toContain('3');
    expect(wrapper.find('#gauge-4').text()).toContain('4');
    expect(wrapper.find('#gauge-5').text()).toContain('5');
    expect(wrapper.find('#gauge-6').text()).toContain('6');
  })

  



})