import { describe, expect, test, mount } from 'vitest'
import NavigationBar from '../../src/components/NavigationBar.vue';
import { createStore } from 'vuex';
import uiStore from '../../src/modules/uiStore.js'

const createVuexStore = () => 

    createStore({
        modules:{
            ui: uiStore,
        }
    });


describe('NavigationBar.vue', () => {
  it('does it render', () => {
    const store = createVuexStore();
    const wrapper = mount(NavigationBar, {
      global:{
        plugins: [store]
      }
    });

    expect(wrapper.find('.navbar').text()).toContain('Remote Lab: Truss Lab');
  })

})