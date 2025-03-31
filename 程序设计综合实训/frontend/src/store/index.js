// src/store/index.js

import { createStore } from 'vuex'
import auth from './modules/auth'
import menu from './modules/menu'
import orders from './modules/orders'

const store = createStore({
  modules: {
    auth,
    menu,
    orders,
  },
})

export default store
