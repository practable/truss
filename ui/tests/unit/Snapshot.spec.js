import { describe, expect, test, it } from 'vitest'
import {mount} from "@vue/test-utils";
import Snapshot from '../../src/components/Snapshot.vue';
import { createStore } from 'vuex';

import dataStore from '../../src/modules/dataStore.js'
import uiStore from '../../src/modules/uiStore.js'

// const createVuexStore = (initialState) => 

//     createStore({
//       state: () => ({
//         current: {},   //{load_cell: 2.0, gauge_1: 0.5 .....}
//         history:[],    //array of objects of form of current above
//         ...initialState       
//       }),
//       mutations:{
//         ADD_TO_HISTORY(state, data){
//             state.history.push(data);
//          },
//          CLEAR_ALL_HISTORY(state){
//           state.history = [];
//           //state.data.length = 0; //all references to data are cleared
//        },
//       },
//       actions:{
//         addToHistory(context, data){
//             context.commit("ADD_TO_HISTORY", data);
//          },
//          clearAllHistory(context){
//           context.commit('CLEAR_ALL_HISTORY');
//        },
//       },
//       getters: {
//         getCurrent(state){
//           return state.current;
//       },
//       getHistory(state){
//         return state.history;
//      }
//       }
//     })

const createVuexStore = () => 

    createStore({
        modules:{
            data: dataStore,
            ui: uiStore
        }
    });
    



describe('Snapshot.vue', () => {
  it('does it render', () => {
    const store = createVuexStore();
    store.dispatch('setCurrent', {load_cell: 0.0, gauge_1: 0.0, gauge_2: 0.0, gauge_3: 0.0, gauge_4: 0.0, gauge_5: 0.0, gauge_6: 0.0});
    const wrapper = mount(Snapshot, {
      global:{
        plugins: [store]
      }
    });

    expect(wrapper.find('#snapshot').text()).toContain('Record Snapshot');
    expect(wrapper.find('#current-data'));
  })

  it('Clicking Record Snapshot adds data', async () => {
    const store = createVuexStore();
    store.dispatch('setCurrent', {load_cell: 999, gauge_1: 0.5, gauge_2: 0.5, gauge_3: 0.5, gauge_4: 0.5, gauge_5: 0.5, gauge_6: 0.5});
    const wrapper = mount(Snapshot, {
      global:{
        plugins: [store]
      }
    });

    const button = wrapper.find('#snapshot-button');
    await button.trigger('click');

    expect(wrapper.find('#history-0').text()).toContain('0.5');

  })

  test('Reset button shows modal', async () => {
    const store = createVuexStore();
    store.dispatch('setCurrent', {load_cell: -12, gauge_1: -0.1, gauge_2: 0.5, gauge_3: 0.5, gauge_4: 0.5, gauge_5: 0.5, gauge_6: 0.5});
    const wrapper = mount(Snapshot, {
      global:{
        plugins: [store]
      }
    });

    const button = wrapper.find('#reset-snaps');
    await button.trigger('click');

    expect(wrapper.find('.modal').exists()).toBe(true);


  });

  test('Add history => Reset button => Modal deletes history', async () => {
    const store = createVuexStore();
    store.dispatch('setCurrent', {load_cell: -12, gauge_1: -0.1, gauge_2: 0.5, gauge_3: 0.5, gauge_4: 0.5, gauge_5: 0.5, gauge_6: 0.5});
    const wrapper = mount(Snapshot, {
      global:{
        plugins: [store]
      }
    });

    const button = wrapper.find('#snapshot-button');
    await button.trigger('click');

    expect(wrapper.find('#history-0').text()).toContain('-0.1');

    const button_reset = wrapper.find('#reset-snaps');
    await button_reset.trigger('click');

    const button_modal = (wrapper.find('.modal-footer')).find('.button-danger');
    await button_modal.trigger('click');
  
    expect(wrapper.find('.modal').exists()).toBe(false);

    //wrapper.vm.$nextTick;

    expect(wrapper.find('#history-0').exists()).toBe(false);


  });

})