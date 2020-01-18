r"""Running a pre-trained ppo agent on minitaur_trotting_env."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
import warnings
import os
import copy
import time
import math
import tensorflow as tf
from pybullet_envs.minitaur.agents.scripts import utility
import pybullet_data
from pybullet_envs.minitaur.envs import simple_ppo_agent
import ruamel

flags = tf.app.flags
FLAGS = tf.app.flags.FLAGS
LOG_DIR = os.path.join(pybullet_data.getDataPath(), "policies/ppo/minitaur_trotting_env")
CHECKPOINT = "model.ckpt-14000000"
warnings.simplefilter('ignore', ruamel.yaml.error.UnsafeLoaderWarning)


def main(argv):
  del argv  # Unused.
  config = utility.load_config(LOG_DIR)
  print(LOG_DIR)
  policy_layers = config.policy_layers
  value_layers = config.value_layers
  env = config.env(render=True)
  network = config.network

  with tf.Session() as sess:
    agent = simple_ppo_agent.SimplePPOPolicy(sess,
                                             env,
                                             network,
                                             policy_layers=policy_layers,
                                             value_layers=value_layers,
                                             checkpoint=os.path.join(LOG_DIR, CHECKPOINT))
    sum_reward = 0
    observation = env.reset()
    while True:
      action = agent.get_action([observation])
      o_action = copy.deepcopy(action)
      o_action = env.transform_action_to_motor_command(o_action[0])
      print("----- each motor radio -----")
      print(o_action)
      print("----- each motor angle -----")
      pi = 3.14159265359
      for each_rad in o_action:
        print(180*each_rad/pi)          
      observation, reward, done, _ = env.step(action[0])
      time.sleep(0.002)
      sum_reward += reward
      if done:
        break
    tf.logging.info("reward: %s", sum_reward)


if __name__ == "__main__":
  tf.app.run(main)
