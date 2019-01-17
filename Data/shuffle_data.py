#!/usr/bin/env python
# coding: utf-8

# In[25]:


import pandas as pd
import numpy as np


# In[26]:


filename = "credit_approval_australia.dat"
data = pd.read_table(filename, comment="#", delim_whitespace=True, header=None)


# In[28]:


np.random.shuffle(data.values)



# In[30]:


data.to_csv("shuffled_"+filename, header=None, index=None, sep=' ')


# In[ ]:





# In[ ]:




