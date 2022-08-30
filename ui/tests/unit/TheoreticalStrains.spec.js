import { mount } from '@vue/test-utils'
import { createStore } from 'vuex';
import TheoreticalStrains from '../../src/components/TheoreticalStrains.vue';

import dataStore from '../../src/modules/dataStore.js'

//import { store } from '../../src/store.js';

const createVuexStore = () => 

    createStore({
      modules:{
        data: dataStore
      }
    })


describe('TheoreticalStrains', () => {
  test('can render with default store', () => {
    const store = createVuexStore();
    store.dispatch('setCurrent', {load_cell: 10.0});
    const wrapper = mount(TheoreticalStrains, {
      global:{
        plugins: [store]
      }
    });
    expect(wrapper.find('#theory-values'));
  })

  test('calculate correct gauge values, positive load', () => {
    const store = createVuexStore();
    store.dispatch('setCurrent', {load_cell: 10.0});
    const wrapper = mount(TheoreticalStrains, {
      global:{
        plugins: [store]
      }
    });

    expect(wrapper.find('#gauge-theory-1').text()).toContain('-22.6');
    expect(wrapper.find('#gauge-theory-2').text()).toContain('16');
    expect(wrapper.find('#gauge-theory-3').text()).toContain('16');
    expect(wrapper.find('#gauge-theory-4').text()).toContain('-16');
    expect(wrapper.find('#gauge-theory-5').text()).toContain('-22.6');
    expect(wrapper.find('#gauge-theory-6').text()).toContain('32');
  })

  test('calculate correct gauge values, negative load', () => {
    const store = createVuexStore();
    store.dispatch('setCurrent', {load_cell: -10.0});
    const wrapper = mount(TheoreticalStrains, {
      global:{
        plugins: [store]
      }
    });

    expect(wrapper.find('#gauge-theory-1').text()).toContain('22.6');
    expect(wrapper.find('#gauge-theory-2').text()).toContain('-16');
    expect(wrapper.find('#gauge-theory-3').text()).toContain('-16');
    expect(wrapper.find('#gauge-theory-4').text()).toContain('16');
    expect(wrapper.find('#gauge-theory-5').text()).toContain('22.6');
    expect(wrapper.find('#gauge-theory-6').text()).toContain('-32');
  })

  test('calculate correct gauge values, 0 load', () => {
    const store = createVuexStore();
    store.dispatch('setCurrent', {load_cell: 0.0});
    const wrapper = mount(TheoreticalStrains, {
      global:{
        plugins: [store]
      }
    });

    expect(wrapper.find('#gauge-theory-1').text()).toContain('0');
    expect(wrapper.find('#gauge-theory-2').text()).toContain('0');
    expect(wrapper.find('#gauge-theory-3').text()).toContain('0');
    expect(wrapper.find('#gauge-theory-4').text()).toContain('0');
    expect(wrapper.find('#gauge-theory-5').text()).toContain('0');
    expect(wrapper.find('#gauge-theory-6').text()).toContain('0');
  })


})